
//////////////////////////////////////////////////// Old Code ///////////////////////////////////////////////////
#if 0
void evalMultipleCommands(const std::string& command, const std::vector<std::string>& vKeys, const std::vector<std::string>& vArgs)
{
    redisReply *reply = ( redisReply * ) redisCommand( _redisContext,
                                                      "EVAL %s %u %s %s %s",
                                                      command.c_str(),
                                                      vKeys.size(),
                                                      vKeys[0].c_str(),
                                                      vKeys[1].c_str(),
                                                      vArgs[0].c_str());
    handleReply(reply);

    freeReplyObject(reply);
}
    std::vector<std::string> vKeys;
    vKeys.push_back("vKey1");
    vKeys.push_back("hKey2");
    std::vector<std::string> vArgs;
    vArgs.push_back("vArgsValue");
    evalMultipleCommands(scriptMultipleCommands, vKeys, vArgs);
#endif

