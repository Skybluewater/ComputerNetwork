//
//  sender.cpp
//  CRC
//
//  Created by Ccy on 2020/3/25.
//  Copyright © 2020 qc. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

int CRC_CCITT[16] = { 0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1 };
int r[16];

void CRC(int p[], int len) {
	int c = 1;
	do {
		if (r[0] == 1) {
			for (int i = 0; i < 15; i++) {
				r[i] = r[i + 1];
			}
			r[15] = p[15 + c];
			for (int i = 0; i < 16; i++) {
				r[i] = r[i] ^ CRC_CCITT[i];
			}
		}
		else {
			for (int i = 0; i < 15; i++) {
				r[i] = r[i + 1];
			}
			r[15] = p[15 + c];
		}
		c++;
	} while (c < len + 1);
}

int main() {
	int p1[64], flag = 0;
	memset(p1, 0, sizeof(p1));
	for (int i = 0; i < 48; i++) {
		char temp;
		cin >> temp;
		p1[i] = temp - '0';
	}
	int len = 48;
	for (int i = 0; i < 16; i++) {
		r[i] = p1[i];
	}
	CRC(p1, len);
	cout << "CRC-Code: ";
	for (int i = 32; i < 48; i++) {
		cout << p1[i];
	}
	CRC(p1, len);
	cout << endl << "余数: ";

	for (int i = 0; i < 16; i++) {
		cout << r[i];
		if (r[i] > 0)
			flag = 1;
	}
	if (flag == 1) {
		cout << " 出错" << endl;
	}
	else {
		cout << " 正确" << endl;
	}
}
