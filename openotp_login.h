#ifndef OPENOTP_LOGIN_H
#define OPENOTP_LOGIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openotp.h>
#include <string>
using namespace std;

void usage(char *prog);
void print_login_response(struct openotp_login_rep_t *rep);
void print_challenge_response(struct openotp_challenge_rep_t *rep);

void _log(char *str);
/*
 ./openotp_login https://147.163.26.241:8443/openotp/ user1.seggio1 -lp password -op 111111

 */
bool otp_login(char url[], char username[], char password[], char insertedOTP[]);


#endif
