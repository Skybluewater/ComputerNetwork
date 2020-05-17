//
//  main.cpp
//  BitStuffing
//
//  Created by Ccy on 2020/3/25.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, const char * argv[]) {
	string k, a, b;
	b = a = "01111110";
	int flag = 0;
	cout << "帧起始标志: " << b << " 帧数据信息: ";
	for (int i = 0; i < 32; i++) {
		char temp;
		cin >> temp;
		k += temp;
		if (temp == '1') {
			flag++;
			if (flag == 5) {
				k.append("0");
				flag = 0;
			}
		}
		else {
			flag = 0;
		}
	}
	a.append(k);
	a.append(b);
	cout << endl << "比特填充后发送帧: " << a << endl;
	int n = 0;
	int num = 0, start = 0, bfor = 0;
	flag = 0;
	string s;
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == '0') {
			if (bfor == 1 && num > 0) {
				bfor = 1;
				s += "0";
				flag = 1;
				start = i;
			}
			else if (n < 5 && num>0 && n > 0) {
				bfor = 1;
				flag = 1;
				s.append(a, start, i - start);
				n = 0;
				start = i;
			}
			else if (n == 5 && num > 0) {
				flag = 0;
				bfor = 0;
				if (i + 1 >= a.length()) {
					cout << "传输出错" << endl;
					break;
				}
				s.append(a, start, i - start);
				n = 0;
				start = i + 1;
			}
			else if (n > 5 && num > 0) {
				cout << "传输出错" << endl;
				break;
			}
			else {
				bfor = 1;
				flag = 1;
				start = i;
			}
		}
		else {
			bfor = 0;
			if (flag == 1) {
				n++;
				if (n == 6) {
					if (i + 1 < a.length()) {
						if (a[i + 1] == '0') {
							if (++num == 2) {
								break;
							}
							n = 0;
							flag = 0;
							i++;
							start = i + 1;
						}
						else {
							cout << "传输出错" << endl;
							break;
						}
					}
					else {
						cout << "error transmission" << endl;
						break;
					}
				}
			}
			else if (flag == 0) {
				n++;
				if (n > 5) {
					cout << "error transmission" << endl;
					break;
				}
			}
		}
	}
	cout << "比特删除的帧数据: " << s << endl;
	return 0;
}
