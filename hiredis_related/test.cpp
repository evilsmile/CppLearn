
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

struct timeval _timeout ;
redisContext *_redisContext;
const long long SEC_TO_USEC = 1000000 ;

void connect(const std::string &ip, int port, int timeoutInUsec )
{
    _timeout.tv_sec = timeoutInUsec / SEC_TO_USEC ;
    _timeout.tv_usec = timeoutInUsec % SEC_TO_USEC ;

    _redisContext = redisConnectWithTimeout(ip.c_str(), port, _timeout);
    if (_redisContext->err)
    {
        std::cout << "Cannot connect to redis server. "
            << " Error : " << _redisContext->errstr
            << std::endl ;
        exit(1);
    }
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
//    std::cout << "Foramt: " << oss_format.str() << std::endl;

    va_list ap;
    va_start(ap, arg_num);
    redisReply *reply = ( redisReply * ) redisvCommand( _redisContext, oss_format.str().c_str(), ap);
    va_end(ap);

    return reply;
}

void testMutex()
{
    std::cout << "=========== test mutex ============..." << std::endl;
    std::string taskId = "78a88bc98-787123cccbdd-2323fffddd";
    std::string key = "timer_task_lock:" + taskId;

    static const std::string component_id = "timer_send_comp_id2";
    static const std::string scriptTestMutext = "local lock_ret = redis.call(\"SETNX\", KEYS[1], ARGV[1]) "
                                   "if lock_ret == 1 then "
                                   "   return ARGV[1] "
                                   "else "
                                   "  local lock_holder = redis.call(\"GET\", KEYS[1]) "
                                   "  return lock_holder "
                                   "end ";
                                   
    redisReply * reply = evalMultipleCommands(1 /* key_num */, 1 /* arg_num */, scriptTestMutext.c_str(), key.c_str(), component_id.c_str());

    if (reply == NULL)
    {
        std::cerr << "NULL redis reply ERROR!" << std::endl;
        return;
    }

    std::string locked_component_id;
    switch(reply->type)
    {
        case REDIS_REPLY_STRING:
            locked_component_id = reply->str;
            break;
        case REDIS_REPLY_ERROR:
            std::cerr << "redis-error when lock task: " << reply->str << std::endl;
            goto error;
        default:
            std::cerr << "redis-error when lock task: Invalid return type=>" << reply->type << std::endl;
            goto error;
    }

    if (locked_component_id != component_id)
    {
        std::cout << "lock failed. holder: " << locked_component_id << std::endl;
    }
    else
    {
        std::cout << "lock succes! holder: " << locked_component_id << std::endl;
    }


error:
    freeReplyObject(reply);
}

void basicTest()
{
    static const std::string scriptSingleCommand = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                                    "return link_id ";

    static const std::string scriptMultipleCommands = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                                      "redis.call(\"HSET\", KEYS[2], link_id, ARGV[1]) "
                                                      "local data = redis.call(\"HGETALL\",KEYS[2]) "
                                                      "return data";

    redisReply* reply = NULL;
    std::cout << "=========== test 1 =========== ..." << std::endl;
    reply = evalMultipleCommands(2 /* key_num */, 0 /* arg_num */, "return {KEYS[1], KEYS[2]}", "key1", "key2");
    handleTestReply(reply);

    std::cout << "=========== test 2 =========== ..." << std::endl;
    reply = evalMultipleCommands(1 /* key_num */, 0 /* arg_num */, scriptSingleCommand.c_str(), "a");
    handleTestReply(reply);

    std::cout << "=========== test 3 =========== ..." << std::endl;
    reply = evalMultipleCommands(2 /* key_num */, 1 /* arg_num */, scriptMultipleCommands.c_str(), "vKey1", "hKey2", "vArgsValue");
    handleTestReply(reply);
}

int main(int argc,char** argv)
{
    connect("127.0.0.1", 6379, 1500000);

    basicTest();

    testMutex();

    return 0;
}
