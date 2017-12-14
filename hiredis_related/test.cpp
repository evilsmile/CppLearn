
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

void handleReply(redisReply* reply)
{
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
}

void evalMultipleCommands(int key_num, int arg_num, ...)
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

    handleReply(reply);

    freeReplyObject(reply);
}

std::string scriptSingleCommand = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                  "return link_id ";

std::string scriptMultipleCommands = "local link_id = redis.call(\"INCR\", KEYS[1]) "
                                     "redis.call(\"HSET\", KEYS[2], link_id, ARGV[1]) "
                                     "local data = redis.call(\"HGETALL\",KEYS[2]) "
                                     "return data";

int main(int argc,char** argv)
{
    connect("127.0.0.1", 6379, 1500000);

    evalMultipleCommands(2 /* key_num */, 0 /* arg_num */, "return {KEYS[1], KEYS[2]}", "key1", "key2");
    evalMultipleCommands(1 /* key_num */, 0 /* arg_num */, scriptSingleCommand.c_str(), "a");
    evalMultipleCommands(2 /* key_num */, 1 /* arg_num */, scriptMultipleCommands.c_str(), "vKey1", "hKey2", "vArgsValue");

    return 0;
}
