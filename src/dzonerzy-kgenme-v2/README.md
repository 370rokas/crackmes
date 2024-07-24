## [dzonerzy's KGenMe v2](https://crackmes.one/crackme/6672bf9ee7b35c09bb266f5e)

### Explaination

First of all analyze the .exe with IDA PRO.

From analyzing the function which checks serial keys format (0x55DD973B34B0) we can learn the format of the keys:
```
// Serial must be 29 chars long, it consists of 5 "parts" of 5 chars, separated with -'s
// 1st part consists of: K, J or L AND 4 digits
// -
// any
// any
// any
// any
// 0, 2, 4, 6, 8
// -
// alphanumeric (A)
// alphanumeric (B)
// digit (C)
// digit (D)						abs((ascii value of A) - (ascii value of B)) == C + D
// any
// -
// not punctuation character
// not punctuation character
// not punctuation character
// not punctuation character
// not punctuation character
// -
// 5th part: there must be atlest 1: H,B,T,O,P    AND    any amount of odd numbers
// Example key: K0000-XXXX0-XX00X-AAAAA-HBTOP
```

Then rewrite the hashing & serial validity check functions in C++. (dont mind my variable & func names, 3am hits differently lol)

Create a function that bruteforces every possible key variation that suits the format.

Add multithreading to meet the 10 minute limit ;p

Bruteforce & profit!

### The cracked credentials

username: DZONERZY

serial: K3750-AAAA0-AA00A-AAVRm-PBPPO

### Test details

Test machine:
```
OS: Ubuntu 22.04.2 LTS on Windows 10 x86_64
Kernel: 5.15.153.1-microsoft-standard-WSL2
CPU: Intel i7-9700 (8) @ 3.000GHz (8 cores allocated to WSL)
Memory: 8GB (allocated to the WSL)
```

Built with: g++ -O3 -march=native -mtune=native -std=c++20 keygen.cpp -o keygen

CLI output:
```
root@desktop:~# g++ -O3 -march=native -mtune=native -std=c++20 keygen.cpp -o keygen
root@desktop:~# time ./keygen
Found: K3750-AAAA0-AA00A-AAVRm-PBPPO

real    0m25.326s
user    3m22.052s
sys     0m0.120s
root@desktop:~# ./kgenmev2
*** Welcome to DZONERZY's keygenme v2! ***

------------------------------------
| Rules:                           |
|                                  |
| - Patching is not allowed!       |
| - Keygen gen time < 10 min       |
| - Comment with proof valid key   |
|   for username 'DZONERZY' and    |
|   result of commant 'time'       |
| - You have to write a keygen!    |
------------------------------------

Enter your username: DZONERZY
Enter your serial: K3750-AAAA0-AA00A-AAVRm-PBPPO
[+] Serial is correct! Now go write a keygen
root@desktop:~#
```
