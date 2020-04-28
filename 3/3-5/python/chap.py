# -*- coding: utf-8 -*-
# 模拟CHAP口令校验

import hashlib
import random
import sys

class CHAPServer(object):

    def __init__(self, key):
        self.key = key.encode(encoding='utf-8')
        self.randNum = bytes()

    #生成随机附加段
    def genRandNum(self, randNumLen = 4):
        rand = [random.randint(0,255) for i in range(randNumLen)]
        self.randNum = bytes(rand)
        return self.randNum

    #校验MD5
    def checkMD5key(self, md5key):
        hl = hashlib.md5()
        hl.update(self.key+self.randNum)
        return(md5key == hl.hexdigest())


# 测试
if __name__ == '__main__':

    if(len(sys.argv) == 2):
        chap = CHAPServer(sys.argv[1])
        print('Key:\t\t',sys.argv[1])

        print('Please input key:')
        inputkey = input()
        print('Input key:\t',inputkey)

        rand = chap.genRandNum()
        print('Random Num:\t',rand.hex())

        #模拟被验证方生成附加段和密钥的MD5
        hl = hashlib.md5()
        hl.update(inputkey.encode(encoding='utf-8')+rand)
        md5key = hl.hexdigest()
        print('MD5:\t\t',md5key)

        #模拟验证方验证MD5
        print('Check Answer:\t',chap.checkMD5key(md5key))

    else:
        print("Wrong Arg Num!")
