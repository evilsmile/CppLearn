
/*----------------------
  EVAL
  ------------------------*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include <sstream>
#include <iomanip>

struct timeval timeout ;
redisContext *_redisContext;
const long long SEC_TO_USEC = 1000000 ;

/* -------------- 计算运行时间函数 -------------- */
static clock_t start,end;
void set_start_time()
{
    start = clock();
}
void calc_run_time(const std::string& msgHeader)
{
    end = clock();

    double run_sec = (double)(end - start)/CLOCKS_PER_SEC;
    std::cout << std::setiosflags(std::ios::fixed);
    std::cout << msgHeader << " spend " << std::setprecision(3) << run_sec << "s" << std::endl;
}

/* -------------- 日志函数 -------------- */
#define CLOSE_LOG
void log(const char* format, ...)
{
#ifdef CLOSE_LOG
    return;
#else
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    printf("\n");
#endif
}

/* -------------- redis相关操作函数  -------------- */
bool connect(const std::string &ip, int port, int timeoutInUsec )
{
    timeout.tv_sec = timeoutInUsec / SEC_TO_USEC ;
    timeout.tv_usec = timeoutInUsec % SEC_TO_USEC ;

    _redisContext = redisConnectWithTimeout(ip.c_str(), port, timeout);
    if (_redisContext->err)
    {
        log("Cannot connect to redis server. Error : %s",  _redisContext->errstr);
        return false;
    }

    return true;
}

void handleTestReply(redisReply* reply)
{
    if (reply == NULL)
    {
        std::cerr << "NULL reply ERROR!" << std::endl;
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY) 
    {
        std::cout << "Get array value:\n[ " << std::endl;
        for (size_t i = 0; i < reply->elements; i++) 
        {
            std::cout<< i <<","<<reply->element[i]->str << std::endl;
        }
        std::cout << "]" << std::endl;
    } 
    else if (reply->type == REDIS_REPLY_STRING) 
    {
        std::cout<<"String value: "<< reply->str << std::endl;
    }
    else if (reply->type == REDIS_REPLY_INTEGER) 
    {
        std::cout<<"Key value "<< reply->integer << std::endl;
    }
    else if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "redis error: " << reply->str << std::endl;
    }
}

redisReply* evalMultipleCommands(int key_num, int arg_num, ...)
{
    // EVAL command key_num
    std::ostringstream oss_format;
    oss_format << "EVAL %s ";
    oss_format << key_num;

    size_t key_arg_num = key_num + arg_num;
    for (size_t i = 0; i < key_arg_num; ++i)
    {
        oss_format << " %s";
    }
    log("Foramt: %s", oss_format.str().c_str());

    va_list ap;
    va_start(ap, arg_num);
    redisReply *reply = ( redisReply * ) redisvCommand( _redisContext, oss_format.str().c_str(), ap);
    va_end(ap);

    return reply;
}


void basicTest()
{
    redisReply* reply = NULL;
#if 0
    log("=========== test 1 =========== ...");
    reply = evalMultipleCommands(2 /* key_num */, 0 /* arg_num */, "return {KEYS[1], KEYS[2]}", "key1", "key2");
    handleTestReply(reply);

    log("=========== test 2 =========== ...");
    static const std::string scriptSingleCommand = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                                    "return link_id ";
    reply = evalMultipleCommands(1 /* key_num */, 0 /* arg_num */, scriptSingleCommand.c_str(), "a");
    handleTestReply(reply);

    log("=========== test 3 =========== ...");
    static const std::string scriptMultipleCommands = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                                      "redis.call(\"HSET\", KEYS[2], link_id, ARGV[1]) "
                                                      "local data = redis.call(\"HGETALL\",KEYS[2]) "
                                                      "return data";

    reply = evalMultipleCommands(2 /* key_num */, 1 /* arg_num */, scriptMultipleCommands.c_str(), "vKey1", "hKey2", "vArgsValue");
    handleTestReply(reply);
#endif
}

void testMutex()
{
    log("=========== test mutex ============...");
    std::string taskId = "78a88bc98-787123cccbdd-2323fffddd";
    std::string key = "timer_task_lock:" + taskId;

    static const std::string component_id = "timer_send_comp_id2";

    static const std::string scriptTestMutext = "local lock_ret = redis.call(\"SETNX\", KEYS[1], ARGV[1]) "
                                   "if lock_ret == 1 then " //lock succ
                                   "   return ARGV[1] "
                                   "else "
                                   "  local lock_holder = redis.call(\"GET\", KEYS[1]) " // lock failed, return current lock holder
                                   "  return lock_holder "
                                   "end ";
                                   
    redisReply * reply = evalMultipleCommands(1 /* key_num */, 1 /* arg_num */, scriptTestMutext.c_str(), key.c_str(), component_id.c_str());

    if (reply == NULL)
    {
        std::cerr << "NULL redis reply ERROR!" << std::endl;
        return;
    }

    std::string lock_holder;
    switch(reply->type)
    {
        case REDIS_REPLY_STRING:
            lock_holder = reply->str;
            break;
        case REDIS_REPLY_ERROR:
            log("redis-error when lock task: %s", reply->str);
            goto error;
        default:
            log("redis-error when lock task: Invalid return type=>%d", reply->type);
            goto error;
    }

    if (lock_holder != component_id)
    {
        log("lock failed. holder: %s", lock_holder.c_str());
    }
    else
    {
        log("lock succes! holder: %s", lock_holder.c_str());
    }


error:
    freeReplyObject(reply);
}


int main(int argc,char** argv)
{
    set_start_time();
    if (!connect("127.0.0.1", 6379, 1500000))
    {
        return -1;
    }
    calc_run_time("connect-redis");

    // ---> lua功能的简单基本测试 <----
    set_start_time();
    basicTest();
    calc_run_time("basic_test");

    // ---> 测试设置加锁 <----
    set_start_time();
    int test_cnt = 100000;
    for (int i = 0; i < test_cnt; ++i)
    {
        testMutex();
    }
    calc_run_time("test-mutex");

    return 0;
}
