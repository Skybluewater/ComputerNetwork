package com.bit;
import java.security.MessageDigest;
import java.util.Random;
import java.util.Scanner;


public class chapdemo {
    public static void main(String[] args) throws Exception {
        int len=args.length;
        if(len != 1)
            System.out.println("Wrong Arg Num!");
        else {
            CHAPServer chap =new CHAPServer(args[0]);
            System.out.printf("Key:\t%s\n",args[0]);

            System.out.println("请输入口令：");
            Scanner sc = new Scanner(System.in);
            String inputkey = sc.nextLine();
            System.out.printf("Input key:\t%s\n",inputkey);

            String rand = chap.genRandNum();
            System.out.printf("Rand Num:\t%s\n",rand);

            String inputs = inputkey + rand;
            MyMD5Util mds = new MyMD5Util();
            mds.md5Hex(inputs);
        }
    }
}
class CHAPServer{
    String key;
    int randNumLen = 4;
    String randNum = "";

    public CHAPServer(String inputs)
    {
        key = inputs;
    }
    public String genRandNum()
    {
        Random random = new Random();
        for(int i=0;i<randNumLen;i++)
        {
            randNum += String.valueOf(random.nextInt(10));
        }
        return randNum;
    }

}

class MyMD5Util {

    public static void md5Hex(String inpu) throws Exception {
        //获的MD5消息摘要
        MessageDigest md5 = MessageDigest.getInstance("MD5");
        md5.update(inpu.getBytes("utf-8"));
        byte[] result = md5.digest();



        String hexString = toHexString(result);
        System.out.println("MD5:"+ hexString);
    }


    public static String toHexString(byte[] bytes) {
        StringBuffer sb = new StringBuffer();
        for (byte b : bytes) {
            String hex = Integer.toHexString(b & 0x0FF);
            if (hex.length() == 1) hex = "0" + hex;
            sb.append(hex);
        }
        return sb.toString();
    }
}
