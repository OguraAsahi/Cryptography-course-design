#include <iostream>
#include <cstdio>
#include <algorithm>
#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/comp.h>
#include <openssl/bio.h>
#include <openssl/sm2.h>
#include <openssl/sm3.h>
#include <openssl/sms4.h>
#include <openssl/sm9.h>
#include <openssl/pkcs7.h>

using namespace std;

bool ifcansign(X509* test);

const char pkeyB[] = "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

const char cacert[] = "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";

char finalresult[10000];
char temp[100010];

EVP_PKEY* getpkey(const char* private_key)
{
	BIO* bio_pkey = BIO_new_mem_buf((char*)private_key, strlen(private_key));
	if (bio_pkey == NULL)
		return NULL;
	return  PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
}

X509* getX509(const char* cert)
{
	BIO* bio;
	bio = BIO_new(BIO_s_mem());
	BIO_puts(bio, cert);
	return  PEM_read_bio_X509(bio, NULL, NULL, NULL);
}


bool verifyCRT(char* varbuf, unsigned int varlen)
{
	BIO* crttest = BIO_new_mem_buf(varbuf, varlen);
	PKCS7* test2 = PEM_read_bio_PKCS7(crttest, NULL, NULL, NULL);
	STACK_OF(X509)* certs = NULL;
	X509* test = getX509(cacert);
	EVP_PKEY* pubkey = X509_get_pubkey(test);
	if (test2 == NULL)
		return 0;
	int i = OBJ_obj2nid(test2->type);
	if (i == NID_pkcs7_signed)
	{
		certs = test2->d.sign->cert;
	}
	else if (i == NID_pkcs7_signedAndEnveloped)
	{
		certs = test2->d.signed_and_enveloped->cert;
	}
	for (i = 0; certs && i < sk_X509_num(certs); i++)
	{
		X509* xx = sk_X509_value(certs, i);
		int finalscore = X509_verify(xx, pubkey);
		if (finalscore != 1)
			return 0;
	}
	EVP_PKEY_free(pubkey);
	return 1;
}
bool verifySign(char* signature_msg, unsigned int length)
{
	BIO* crttest = BIO_new_mem_buf(signature_msg, length);
	EVP_PKEY* pKey = getpkey(pkeyB);
	PKCS7* p7 = PEM_read_bio_PKCS7(crttest, NULL, NULL, NULL);
	BIO* p7bio = PKCS7_dataDecode(p7, pKey, NULL, NULL);
	char tmp[1000];
	BIO_read(p7bio, tmp, 1000);
	STACK_OF(PKCS7_SIGNER_INFO)* sk = PKCS7_get_signer_info(p7);
	int signcount = sk_PKCS7_SIGNER_INFO_num(sk);
	for (int i = 0; i < signcount; i++)
	{
		PKCS7_SIGNER_INFO* signInfo = sk_PKCS7_SIGNER_INFO_value(sk, i);
		STACK_OF(X509)* certs = NULL;
		certs = p7->d.signed_and_enveloped->cert;
		X509* cert = X509_find_by_issuer_and_serial(certs, signInfo->issuer_and_serial->issuer, signInfo->issuer_and_serial->serial);
		if (!ifcansign(cert))
			return 0;
		if (PKCS7_signatureVerify(p7bio, p7, signInfo, cert) != 1)
			return 0;
	}
	return 1;
}



bool ifcansign(X509* test)
{
	ASN1_BIT_STRING* test2;
	unsigned short superid;
	int cmp = X509_get_ext_by_NID(test, NID_key_usage, 0);
	test2 = (ASN1_BIT_STRING*)X509_get_ext_d2i(test, NID_key_usage, NULL, NULL);
	if (test2)
	{
		superid = test2->data[0];
		if (test2->length > 1)
		{
			superid |= test2->data[1] << 8;
		}
		if (superid & X509v3_KU_DIGITAL_SIGNATURE)
			return true;
	}
	return false;
}

void pkcs7decrypt2(unsigned char* testbuf, int testlen)
{
	OpenSSL_add_all_algorithms();
	EVP_PKEY* pKey = getpkey(pkeyB);
	BIO* data = BIO_new_mem_buf(testbuf, testlen);
	PKCS7* pkcs = PEM_read_bio_PKCS7(data, NULL, NULL, NULL);
	BIO* bio_out = PKCS7_dataDecode(pkcs, pKey, NULL, NULL);
	int len = BIO_read(bio_out, finalresult, 10000);
	if (len <= 0)
	{
		printf("ERROR");
		return;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			printf("%c", finalresult[i]);
		}
	}
	return;
}

int main()
{
	char linetemp[1010];
	while (!cin.eof())
	{
		cin.getline(linetemp, 1000);
		strcat(temp, linetemp);
		strcat(temp, "\n");
	}
	int lenx = strlen(temp);
	bool judge = verifyCRT(temp, lenx);
	if (judge == 0)
	{
		printf("ERROR");
		return 0;
	}
	bool judge2 = verifySign(temp, lenx);
	if (judge2 == 0)
	{
		printf("ERROR");
		return 0;
	}
	pkcs7decrypt2((unsigned char*)temp, lenx);
	return 0;
}