#include<cstdio>
#include<cstdlib>
#include<Windows.h>
#include<iostream>
#include<winternl.h>
#include<assert.h>
//#define CHUTI 1
using namespace std;
string flag;
//string ans = "BUAACTF{Congratulation!GoodJob!}";
//string flag1 = "BUAACTF{Congratu";
//string flag2 = "lation!GoodJob!}";
int crc;
const int LEN = 32;
const int N = 221; // 17*13
const int E = 47;
//const int 
int test(int a,int b);
void DebugDetect();
int qpow(int a, int b);
DWORD Hash(int x);
void cal() ;
int check();
int checkk();
BYTE enc1(BYTE x);
void enc2(DWORD* v);
//int check2();
void DebugDetect(){
	int is_debugged = 0;
	BOOL v0; // esi
	v0 = IsDebuggerPresent();
	if (v0) {
		MessageBoxA(NULL, "Debug Detected!", "Warning", MB_OKCANCEL);
		exit(-1);
	}
//	cout << test(1, 2)<<"\n";
}
int qpow(int a, int b) {
	int tmp = 1;
	while (b) {
		if (b & 1)tmp *= a;
		a *= a;
		b >>= 1;
	}
	return tmp;
}
int powmod(int a,int b,int c){
	int tmp = 1;
	while (b) {
		if (b & 1)tmp *= a,tmp = (tmp%c+c)%c;
		a *= a;
		a = (a%c+c)%c;
		b >>= 1;
	}
	return tmp;
}
DWORD Hash(int x) {
label:
	int* addr = (int *)DebugDetect;
	BYTE* staddr = (BYTE*)addr;
	int key1 = 0, key2 = 0;
	for (int i = 0; i < 4; i++)
		key1 ^= *(staddr + i);
	for (int i = 4; i < 9; i++)
		key2 ^= *(staddr + i);
	DWORD y = qpow(x, key1);
	y += qpow(x, 5);
	y += qpow(key2, key1 / 10) * qpow(x, key2 - min(key2, 14));
	return y;
}
typedef struct{
	DWORD hash;
	BYTE enc_type;
	DWORD size;
	BYTE data[20];
	BYTE sign[10]={'-','N','0','z','o','M','1','z','0','-'};
//	string sign = "-N0zoM1z0-";
}EncData;
BYTE Right_Sign[11]={'-','N','0','z','o','M','1','z','0','-'};
void cal() {
	crc = 0;
	
	int* addr = (int*)DebugDetect;
//	cout << addr << "\n";
	BYTE* staddr = (BYTE *)addr;
	for (int i = 0; i < 72; i++)
		crc += *(staddr+i);
	//cout << crc << "\n";
//	cout << Hash(crc) << "\n";
	if (Hash(crc) != 3283837792) {
//		cout << Hash(crc) << "\n";
		MessageBoxA(NULL, "Patch Detected!", "Warning", MB_OKCANCEL);
		exit(-1);
	}
	return ;
}
#ifdef CHUTI
void encrypt(string s){
	EncData* encdata1 = new EncData();
	EncData* encdata2 = new EncData();
	// enc1
	FILE* out;
	out = fopen("enc1","wb");
	
	BYTE* data = encdata1->data;
	for(register int i=0;i<16;i++){
		BYTE x = (BYTE)(flag1[i]);
		x = enc1(x);
		data[i] = x;
		assert((encdata1->data)[i]==data[i]);
	}
	encdata1->enc_type = 1;
	encdata1->size = 16;
	srand(0x1024);
	encdata1->hash = Hash(rand());
	for(register int i=0;i<16;i++){
		data[i] ^= encdata1->hash^0x4D;
	}
	fwrite(encdata1,1,sizeof(EncData),out);
	fclose(out);
	// enc2
	void *p2 = (void *)flag2.c_str();
	DWORD* data2 = (DWORD*)p2;
//	cout<<"data2: \n";
	for(register int i=0;i<2;i++){
//		cout<<(*(data2+2*i))<<" "<<(*(data2+2*i+1))<<"     ";
		enc2(data2+2*i);
//		cout<<(*(data2+2*i))<<" "<<(*(data2+2*i+1))<<"\n";
	}
//	cout<<"\n";
	encdata2->enc_type = 2;
	encdata2->hash = Hash(rand());
	encdata2->size = 16;
	BYTE* pdata2 = (BYTE*)data2;
	for(register int i=0;i<16;i++){
		BYTE x = *(pdata2+i)^(encdata2->hash&0xff);
		encdata2->data[i] = x;
	}
	out = fopen("enc2","wb");
	fwrite(encdata2,1,sizeof(EncData),out);
	fclose(out);
}
#endif
int check(){
	if(flag.length()!=32){
//		cout<<"Wrong Length!\n";
		return 0;
	}
//	cout<<"Right Length\n";
	if(checkk()){
		return 1;
	}
	else{
//		cout<<"Wrong!\n";
		return 0;
	}
	
}
void enc2(DWORD* v){
	const int ROUNDS = 32;
	const DWORD DELTA = 0x9e3779b9;
	DWORD v0 = v[0],v1 = v[1];
	DWORD k0 = 0x1,k1 = 0x2,k2 = 0x3,k3 = 0x4,sum = 0;
//	if(crc!=3283837792)exit(0);
	for(int i=0;i<ROUNDS;i++){
		sum += DELTA;
		v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
	}
	v[0] = v0,v[1] = v1;
}
BYTE enc1(BYTE x){
	for(register int i=0;i<5;i++)
		x = powmod(x,E,N);
	return (x&0xff);
}
int checkkk(EncData encdata,string input){
	if(memcmp(Right_Sign,encdata.sign,10)){
		cout<<"Invalid Sign!\nPlease check encfiles's format!";
		system("pause");
		exit(0)	;
	}
//	cout<<"Valid SIgn!\n";
	BYTE c[20];
	int type = encdata.enc_type;
//	for(int i=0;i<16;i++)
//		printf("%d ",encdata.data[i]);
//	cout<<"\n";
//	cout<<type<<"   ";
	if(type==1){
		int ok = 1;
//		if(crc!=3283837792)exit(0);
		BYTE* data = encdata.data;
		for(int i=0;i<16;i++){
			c[i] = enc1(input[i]);
		}
		for(int i=0;i<16;i++){
			c[i] ^= encdata.hash^0x4D;
			c[i]&=0xff;
			if(Hash(crc) != 3283837792)return 0;
			if(c[i]!=data[i]){
				ok = 0;
			}
		}
//		for(int i=0;i<16;i++)
//		printf("%d ",c[i]);
//		cout<<"\n";
//		if(ok)cout<<"Right in Part1\n";
		return ok;
	}
	else if(type==2){
		int ok = 1;
		string flagg = "";
//		if(crc!=3283837792)exit(0);
		if(Hash(crc) != 3283837792)return 0;
		for(int i=16;i<32;i++)flagg+=flag[i];
		void* p = (void*)flagg.c_str();
		DWORD* data2 = (DWORD*)p;
//		cout<<"\n";
		for(int i=0;i<2;i++){
			enc2(data2+2*i);
		}
		BYTE* pdata2 = (BYTE*)data2;
		for(int i=0;i<16;i++){
			pdata2[i] ^= encdata.hash;
//			printf("%d ",pdata2[i]);
			if(pdata2[i]!=encdata.data[i]){
				ok = 0;
			}
		}
//		cout<<"\n";
//		if(ok==1)cout<<"Right in Part2!\n";
		return ok;
	}
	else{
		cout<<"Something Wrong...\n";
		return 0;
	}
}
int checkk(){
//	cal();
	string input = flag;
	BYTE c[20];
	FILE* in;
	EncData p;
	in = fopen("enc1","rb");
	if(in==NULL){
		cout<<"Error opening enc1!\n";
		exit(-1);
	}
	fread(&p,1,sizeof(EncData),in);
	fclose(in);
	EncData encdata = (EncData)p;
	int ok1= checkkk(encdata,input);
	
	in = fopen("enc2","rb");
	if(in==NULL){
		cout<<"Error opening enc2!\n";
		exit(-1);
	}
	EncData pp;
	fread(&pp,1,sizeof(EncData),in);
	fclose(in);
	EncData encdata2 = (EncData)pp;
	int ok2 = checkkk(encdata2,input);
	if(Hash(crc) != 3283837792)return 0;
	if(ok1==1&&ok2==1)return 1;
	return 0;
}
signed main() {
	DebugDetect();
	cal();
	cout<<"PLZ input your flag: ";
	cin>>flag;
//	cout<<flag<<"\n";
//	encrypt(flag);
	if(check())
		cout<<"WellDone!\n";
	else cout<<"PLZ try again~\n";
//	cout<<Hash(crc)<<"\n";
	system("pause");
}