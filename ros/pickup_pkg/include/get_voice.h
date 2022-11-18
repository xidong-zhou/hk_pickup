/*
 *      Author: zxd
 * Create Date: 2022-11-15
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <termio.h>
using namespace std;
char* Get_Voice(unsigned int mode, int &size);
int scanKeyboard();
void Record_Voice(vector<unsigned char> vec_in);




