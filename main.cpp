#include <string>
#include <iostream>
#include "mlcs.h"

using namespace std;

int main()
{
	unsigned long idx, jdx, shift = 0;
//	string s1 = "okfhgierhighiioejkfuhkjgoijljgredr";
//	string s2 = "lkjghoeirgjokfgirehgkherlelhre";
//	string s3 = "hkerjgoewrjgoejfdhiuehrkheherhkekertr";
	string s1 = "TGACGATC";
	string s2 = "ATGCTCAG";
	string s3 = "CTAGTACG";
//	string s1 = "okfhgierhighiioejkfuhkjgoillkdeiehkdkjkiehrgjksdkgdskglksdjghedgksdjhgdsklhklsdlgjljgredr";
//	string s2 = "lkjghoeirgjokfgirehgdskhjgkdshgklgkhldshlkdjsijglieslsdklldjerjlkjlgkdslgjldgldskherlelhre";
//	string s3 = "hkerjgoewrjgoejfdhiuldskhgleisglkdgldkhsglsdljgihgdksjlsjsirejkslkjgliejgljdskljljghslkfdhfdhssgdsgsrhfsdsdehrkheherhkekertr";

	string strSum = s1 + s2 + s3;
	unsigned long testLengths[3] = { s1.length(), s2.length(), s3.length() };
	unsigned long testNumber = 3;
	unsigned short* data = new unsigned short[strSum.length()];
	
	for (idx = 0; idx < strSum.length(); idx++)
	{
		data[idx] = strSum[idx];
	}

	short dataPreprocessed = ComputeMCLSs(data, testLengths, testNumber, true, 0);
	unsigned long subSeqNumber;
	unsigned long* subSeqLengths;

	unsigned short* subSeq = get_subsequences(&subSeqLengths, &subSeqNumber);

	cout << "sub sequences:" << endl;
	for (idx = 0; idx < subSeqNumber; idx++)
	{
		for (jdx = 0; jdx < subSeqLengths[idx]; jdx++)
		{
			cout << static_cast<char>(subSeq[shift + jdx]);
		}
		shift = shift + subSeqLengths[idx];
		cout << endl;
	}

 	return 0;
}