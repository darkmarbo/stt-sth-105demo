#include "afx.h"
#include "word_seg_lib.h"
#include "stdio.h"
#include <string.h>
#include "stdafx.h"
#include "Dictionary.h"
#include "MaxMatch.h"
#include "UTF8Util.h"
#include "MaxMatch_Engine.h"
#include "cppjieba/Jieba.hpp"

using namespace std;
std::map<std::string, std::string> map_pos;  // ���� pos ת���� map_list 
std::map<std::string, std::string> map_pos_set; // ���ɴ�ģ������ȡ������ ���Լ��б� 
const int MAX_LINE = 10000;

MaxMatch_Engine maxMatch_engine;
cppjieba::Jieba *g_jieba = NULL;


#if defined(_MSC_VER) && _MSC_VER > 1310
# include <Windows.h>
# define utf8(str) ConvertToUTF8(L ## str)
string ConvertToUTF8(const wchar_t* pStr) {
	static char szBuf[1024];
	WideCharToMultiByte(CP_UTF8, 0, pStr, -1, szBuf, sizeof(szBuf), NULL, NULL);
	return szBuf;
}

#else // if defined(_MSC_VER) && _MSC_VER > 1310
// Visual C++ 2003 and gcc will use the string literals as is, so the files
// should be saved as UTF-8. gcc requires the files to not have a UTF-8 BOM.
# define utf8(str) string(str)
#endif // if defined(_MSC_VER) && _MSC_VER > 1310

// const wchar_t * ת���� string ���� char * 
std::string CWTOA(const wchar_t* lpwcszWString)
{
	char* pElementText;//����һ��char����ָ��
	int iTextLen;//���峤��

	iTextLen = ::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, NULL, 0, NULL, NULL);//��ȡ�����ַ�������


	pElementText = new char[iTextLen + 1];//���ٿռ�
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));//���

	// const wchar_t* ת����  char * 
	::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);//ת��
	// ��Ӧ���� MultiByteToWideChar(__in UINT     CodePage,__in DWORD    dwFlags,__in_bcount(cbMultiByte) LPCSTR   lpMultiByteStr,__in int      cbMultiByte,__out_ecount_opt(cchWideChar) __transfer(lpMultiByteStr) LPWSTR  lpWideCharStr,__in int      cchWideChar);

	std::string strReturn(pElementText);
	
	delete[] pElementText;
	
	return strReturn;

}


/*
	�������ƥ�� ���ķִ� UTF8���� 
	���ִ�֮��ġ����ʡ����²��
	�л����񹲺͹�/n ���л�����/n ���͹�/n
*/

int wordseg_so(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;	
	vector<string> vec_tmp;
	vector<string>::iterator it;

	vec_tmp = maxMatch_engine.seg_str2vec(str_word);

	for (it = vec_tmp.begin(); it != vec_tmp.end();it++)
	{
		str_out += *it + "/" + str_pos + " ";
	}
	

	return ret;
}


//ע�⣺���ַ���Ϊ��ʱ��Ҳ�᷵��һ�����ַ���  
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}

/*
	��ȡ  Data/map_pos.txt �� ȫ��map: map_pos �� 
*/
int load_map_pos(const char *file)
{
	FILE *fp = fopen(file,"r");
	if(fp == NULL)
	{
		printf("read %s failed!\n",file);
		return -1;
	}
	char line[1000] = "\0";
	while(fgets(line, 1000, fp))
	{
		// line ��Ҫɾ��ĩβ�� \r\n \n 

		// ude1	DE	u	�� ��
		// ��һ�� �� ������ 
		std::string str = line;
		int pos_1 = str.find('\t');
		if(pos_1 == NULL)
		{
			printf("map_pos error!\n");
			return -1;
		}

		// �õ��� word=ude1
		std::string word = str.substr(0, pos_1);

		int pos_2 = str.find('\t',pos_1+1); //DE	u	�� ��
		str = str.substr(pos_2+1);//u	�� ��
		int pos_end = str.find('\t');
		if(std::string::npos != pos_end)
		{
			str = str.substr(0,pos_end);
		}
		
		//printf("read line is:%s",line);
		//printf("pos:%s:pos:%s\n",word.c_str(), str.c_str());

		if(str[str.length()-1] == 0x0A)
		{
			str = str.substr(0,str.length()-1);
		}
		map_pos[word] = str;
	}

	std::map<std::string,std::string>::iterator it;
	printf("print map_pos!\n");
	for(it=map_pos.begin(); it != map_pos.end(); it++)
	{
		printf("###%s--->%s",it->first.c_str(), it->second.c_str());
	}
	printf("print map_pos over!\n");
	return 0;
}

/*
��ȡ  Data/map_pos_set.txt �� ȫ��map: map_pos_set ��
*/
int load_map_pos_set(const char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL)
	{
		printf("read %s failed!\n", file);
		return -1;
	}
	char line[1000] = "\0";
	while (fgets(line, 1000, fp))
	{

		std::string str = line;
		int pos_1 = str.find('\t');
		if (pos_1 == NULL)
		{
			printf("map_pos_set error!\n");
			return -1;
		}

		// �õ��� word=ude1
		std::string word = str.substr(0, pos_1);
		map_pos_set[word] = word;

	}

	if (fp != NULL){ fclose(fp); fp = NULL; }

	return 0;

}

int init_nlpir(const char *data_dir, const char *user_word_dict)
{
	int nCount;
	int ret = 0;
	
	// ��ʼ�� jieba
	const char* const DICT_PATH = "data/jieba/jieba.dict.utf8";
	const char* const HMM_PATH = "data/jieba/hmm_model.utf8";
	const char* const USER_DICT_PATH = "data/jieba/user.dict.utf8";
	const char* const IDF_PATH = "data/jieba/idf.utf8";
	const char* const STOP_WORD_PATH = "data/jieba/stop_words.utf8";
	g_jieba = new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
	if (NULL == g_jieba)
	{
		return -1;
	}
	//cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);


	////��ʼ���ִ����
	////�����ڵ�ǰ·���£�����ΪUTF8����ķִ�
	//if(!NLPIR_Init(data_dir))
	//{
	//	printf("ICTCLAS INIT FAILED!\n");
	//	return -2;
	//}
	//
	//nCount = NLPIR_ImportUserDict(user_word_dict);

	char map_pos_file[1000];
	_snprintf(map_pos_file, 1000, "%s/Data/map_pos.txt",data_dir);
	ret = load_map_pos(map_pos_file);
	if(ret <0)
	{
		printf("load_map_pos errer!\n");
		return -3;
	}

	char map_pos_file_set[1000];
	_snprintf(map_pos_file_set, 1000, "%s/Data/map_pos_set.txt", data_dir);
	ret = load_map_pos_set(map_pos_file_set);
	if (ret <0)
	{
		printf("load_map_pos_set errer!\n");
		return -4;
	}

	char dict_so[1000];
	_snprintf(dict_so, 1000, "%s/Data/dict_so.txt", data_dir);
	ret = maxMatch_engine.init(dict_so);

	return ret;
}

/*
// jieba �ִ�����Ա�ע  ʵ��ʹ��UTF8����
// ���ʴ���  ����ת��
*/
int SplitGBK(const char *sInput, char *sOutput, int size)
{

	const char * sResult;
	char dest[MAX_LINE] = { 0 };
	// ���� split��� 
	std::vector<std::string> vec_spl_wp;
	std::vector<std::string> vec_wp;
	std::string delim = " ";
	std::string delim_2 = "//";
	std::vector<std::string>::iterator it;
	std::string str_out = "";

	// jieba
	vector<pair<string, string> > tagres;
	g_jieba->Tag(sInput, tagres);
	for (vector<pair<string, string> >::iterator it = tagres.begin(); it != tagres.end(); it++)
	{
		if (it->first.size() == 0 || it->first.compare(" ") == 0)
		{
			continue;
		}
		if (it != tagres.begin())
		{
			strcat(dest, " ");
		}
		
		strcat(dest, it->first.c_str());
		strcat(dest, "/");
		strcat(dest, it->second.c_str());
	}
	

	/*
	// �ִ�����Ա�ע �п�Ժ 
	// ��ʼ���������趨���� UTF8 
	sResult = NLPIR_ParagraphProcess(sInput, 1);
	if (sResult == NULL)
	{
		printf("NLPIR_ParagraphProcess failed!\n");
		return -1;
	}

	_snprintf(dest, MAX_LINE, "%s", sResult);
	*/

	std::string str_tmp = dest;

	// ���տո��з� 
	split(str_tmp, delim, &vec_spl_wp);


	// ȥ�������� word/pos ��ʽ��  �����posת�� 
	for (it = vec_spl_wp.begin(); it != vec_spl_wp.end(); it++)
	{
		//printf("test:%s\n", it->c_str());
		std::vector<std::string> vec_tmp;
		std::string str_tmp = *it;

		// �зֳ� word �� pos 
		split(str_tmp, delim_2, &vec_tmp);
		if (vec_tmp.size() < 2)
		{
			continue;
		}

		std::string str_word = vec_tmp[0]; // �ִ� 
		std::string str_pos = vec_tmp[1];  //  ����

		// �����滻 
		std::map<std::string, std::string>::iterator it;
		it = map_pos.find(str_pos);
		// ��׼���Լ� list 
		std::map<std::string, std::string>::iterator it_set;
		it_set = map_pos_set.find(str_pos);

		// ��ת������ ���� ���ڱ�׼���Լ��� 
		if (it != map_pos.end() && it_set == map_pos_set.end())
		{
			str_pos = it->second;
		}

		// �������м��
		it_set = map_pos_set.find(str_pos);
		// ���ջ��ǲ���list�еĴ���
		if (it_set == map_pos_set.end())
		{
			printf("error pos=%s\n", str_pos.c_str());
		}


		// ���ʴ��� UTF8���� 
		int nLen = str_word.size();
		// UTF8 3��byte����һ���ַ� 
		if (nLen>22)
			//if (nLen>12)
		{
			wordseg_so(str_word, str_out, str_pos);
		}
		else
		{
			str_out += str_word + "/" + str_pos + " ";
		}
		// split_long_word_4(str_word, str_out, str_pos);
		//split_long_word_2(str_word, str_out, str_pos);


	}

	// ���� ���ս�� 
	_snprintf(sOutput, size, "%s", str_out.c_str());


	return 0;
}


/*
	���ִ�֮��ġ����ʡ����²��  GBK 
	�л����񹲺͹�/n ���л�����/n ���͹�/n
*/
int split_long_word_4(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;
	

	// ���� ����8�Ĵ�  (9 10 11 12 ) ��� // 2+2+2+2+x    ������ǧ�İ���ʮ��Ԫ
	if (str_word.size()>16)
	{
		// �л����񹲺͹��ܷ� : �л�����  ���͹���  �� 
		std::string str_word_1 = str_word.substr(0, 8); // ǰ4����
		std::string str_word_2 = str_word.substr(8, 8); // �м� 4���� 
		std::string str_word_x = str_word.substr(16, str_word.size() - 16);

		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>8) 
	{
		// һ���ǧ���� : һ���ǧ ����
		std::string str_word_1 = str_word.substr(0, 8);
		std::string str_word_x = str_word.substr(8, str_word.size() - 8);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else
	{
		// �����4���� �� һ���ǧ 
		str_out += str_word + "/" + str_pos + " ";

	}

	return ret;
}

/*
���ִ�֮��ġ����ʡ����²��  GBK 
�л����񹲺͹�/n ���л�/n  ����/n ����/n ��/n 
*/
int split_long_word_2(const std::string &str_word, std::string &str_out, const std::string &str_pos)
{
	int ret = 0;


	// ���� ����8�Ĵ�  (9 10 11 12 ) ��� // 2+2+2+2+x    ������ǧ�İ���ʮ��Ԫ
	if (str_word.size()>16) 
	{
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_3 = str_word.substr(8, 4);
		std::string str_word_4 = str_word.substr(12, 4);
		std::string str_word_x = str_word.substr(16, str_word.size()-16);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_3 + "/" + str_pos + " ";
		str_out += str_word_4 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>12) //(7,8)  //2+2+2+x 
	{
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_3 = str_word.substr(8, 4);
		std::string str_word_x = str_word.substr(12, str_word.size()-12);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_3 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	else if (str_word.size()>8) //(5,6)  //2+2+x 
	{
		int tmp_size = str_word.size();
		std::string str_word_1 = str_word.substr(0, 4);
		std::string str_word_2 = str_word.substr(4, 4);
		std::string str_word_x = str_word.substr(8, str_word.size()-8);
		str_out += str_word_1 + "/" + str_pos + " ";
		str_out += str_word_2 + "/" + str_pos + " ";
		str_out += str_word_x + "/" + str_pos + " ";
	}
	//else if (str_word.size()>6) //(4) // 2+x
	//{
	//	std::string str_word_1 = str_word.substr(0, 4);
	//	std::string str_word_2 = str_word.substr(4, str_word.size()-4);
	//	str_out += str_word_1 + "/" + str_pos + " ";
	//	str_out += str_word_2 + "/" + str_pos + " ";
	//}
	else
	{
		str_out += str_word + "/" + str_pos + " ";
	
	}

	return ret;
}







//// �ִ�����Ա�ע
//// ���ʴ���  ����ת��
//int SplitGBK(const char *sInput, char *sOutput, int size)
//{
//
//	const char * sResult;
//	char dest[MAX_LINE] = {0};
//
//	sResult = NLPIR_ParagraphProcess(sInput,1);
//	if(sResult == NULL)
//	{
//		printf("NLPIR_ParagraphProcess failed!\n");
//		return -1;
//	}
//
//	_snprintf(dest, MAX_LINE, "%s", sResult);
//	printf("SplitGBK read line:%s\n",dest);
//
//	char *pos = dest;
//	char *dest_st = sOutput;
//	while(*pos != '\0' && *pos != '\n')
//	{
//		if(*pos == '/')
//		{
//			char *pos_find_space = strchr(pos,' ');
//			if(pos_find_space == NULL)
//			{
//				printf("pos format err!\n");
//				return -1;
//			}
//
//			char temp_name[100] = {0};
//			strncpy(temp_name, pos, pos_find_space-pos);
//
//			//printf("....%s",temp_name);
//			std::map<std::string,std::string>::iterator it;
//			it = map_pos.find(std::string(temp_name));
//			if(it != map_pos.end())
//			{
//				//printf(":::%s",temp_name);
//				_snprintf(temp_name,100,"%s",it->second.c_str());
//				//printf("--->%s:::",temp_name);
//			}
//
//			_snprintf(dest_st, size-strlen(dest_st), "%s ", temp_name);
//			dest_st += strlen(temp_name)+1;
//			pos = pos_find_space + 1;
//
//
//		}
//		else
//		{
//			strncpy(dest_st, pos, 2);
//			pos++;
//			dest_st++;
//		}
//	}
//
//
//	return 0;
//
//}






int delete_nlpir()
{
	//NLPIR_Exit();
	if (NULL != g_jieba)
	{
		delete g_jieba;
		g_jieba = NULL;
	}
	return 0;
}
