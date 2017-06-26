#include "afx.h"
#include "tts_all.h"
#include <stdio.h>

/*
����ִʺ���ַ����� // �ֱ�/d ��/v ������ǧһ����ʮ/m Ԫ/q
://�ֱ�/d ��/v ����/m ��ǧ/m һ��/m ��ʮ/m Ԫ/q
*/
int split_long_word()
{
	return 0;
}

/*
	���� model\LC\model-like-77  ֱ�Ӿ�����ѧģ��
	ֻ��ttsdll�õ��˴� Ŀ¼
	���� tn str_pro �ִ� ��д����data������ 
*/
int TTS_ALL::init(const char *model_dir)
{
	int ret = 0;
	char *tn_dir = new char[1000];

	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}
	fp_log = fopen("word_seg.log", "w+");

	// �ַ��� Ԥ����
	_snprintf(tn_dir, 999, "data/str_pro");
	ret = this->m_pp.init(tn_dir);
	if (ret < 0){ return -1; }

	// tn ��ʼ��

	_snprintf(tn_dir, 999, "data/tn");
	ret = m_tn_engine.Init(std::string(tn_dir));
	if (ret < 0){ return -1; }

	// �ִʺʹ��Ա�ע ��ʼ��
	char *uwd = new char[1000];
	_snprintf(uwd, 999, "data/Data/user_word_dict.txt");
	ret = init_nlpir("data", uwd);
	if (ret < 0)
	{
		printf("init_nlpir failed!\n");
		return -1;
	}
	this->nlpir_flag = 1; // û��NLPIR ����jieba��  

	// tts ��ʼ��
	ret = m_tts.init(model_dir);
	if (ret < 0)
	{
		printf("tts init failed!\n");
		return -1;
	}

	delete uwd;
	delete tn_dir;
	return 0;
}

/*
	// ������β���� �ҵ���һ��valueֵ����min_val�ĵ�  Ȼ�������ƶ�400ms(0.4*44100) 
	//buff: buff����  ����Ϊlen
	// left:���˵��idx   right:�Ҳ�˵��idx  
	// min_val: �ж���������ֵ 400 
*/
int TTS_ALL::silence_pro(const short *buff, int len, int &left, int &right, short min_val)
{
	int ret = 0;
	left = 0;
	right = 0;
	short thr = 400;
	int pos = 4 * 4410; // 400ms

	for (int i = 0; i < len;i++)
	{
		if (abs(buff[i]) > thr)
		{
			left = i - pos;
			left = (left > 0 ? left : 0);
			break;
		}
	}
	for (int j = len - 1; j > 0;j--)
	{
		if (abs(buff[j]) > thr)
		{
			right = j + pos;
			right = (right<len ? right : len-1);
			break;
		}
	}

	return ret;
}

/*
	// �������UTF-8�ı�������ִ�У�TN,�Ͼ䣬�ִʡ����ԡ�ת��GBK 
	// �Ե������� ������β��������
	// input: �����ı�
	// buff_size: buff����󳤶� 
*/
int TTS_ALL::tts(const char *input, short *buff, int buff_size)
{

	int ret = 0;
	int len = 0;
	int len_write = 0;  // д��buff���ܳ���
	char *delim = "\n";
	
	char *tmp;
	char lines[MAX_LINE_SIZE];
	char input_new[MAX_LINE_SIZE];
	char * input_new_tmp = input_new;
	short *buff_tmp = new short[WAV_SIZE];
	

	char line_split_tmp1[MAX_LINE_SIZE];
	char line_split_tmp2[MAX_LINE_SIZE];
	std::string str_1;
	std::string str_2;
	fprintf(fp_log, "input_char*:%s\n", input);

	FILE *fp_test_out = fopen("temp.txt","w");
	if (fp_test_out == NULL){ return -1; }
	fprintf(fp_test_out, "%s", input);
	fclose(fp_test_out);
	fp_test_out = fopen("temp.txt", "r");
	

	
	while ( (tmp = fgets(lines, MAX_LINE_SIZE,fp_test_out)) != NULL )
	{
		str_1 = lines;

		// TN ����
		fprintf(fp_log, "=split_str1:%s\n", str_1.c_str());
		str_2 = m_tn_engine.ProcessText(str_1);
		fprintf(fp_log, "tn:%s\n", str_2.c_str());

		// �ַ�������  �Ͼ䣡 �ڲ����̾��Ӻϲ��ɳ����� ��
		// long ���̾��Ӻϲ�  small ��������
		std::vector<std::string> vec_out;

		// ʹ��small��ʱ��  �̾���  kehu-165����֡��̾��ӵĿ�ͷ���ֶ�ʧ����
		//ret = this->m_pp.pre_pro_small(str_2, vec_out);
		
		ret = this->m_pp.pre_pro_long(str_2, vec_out, 3); // 50���� 
		//if (ret < 0){ return -1; }

		// ÿһ���ַ��� �����������ַ�
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{
			
			str_1 = *it_vec;
			fprintf(fp_log, "pre_pro:%s\n", str_1.c_str());
		
			// ֱ��ʹ�� UTF8 
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_1.c_str());
			str_trim(line_split_tmp1);

			// add  ����ִʺʹ��Ա�ע��  �ر𳤵Ĵ� һ�پ�ʮ�� �� ��Ԫ���л����񹲺͹�  �� 
			SplitGBK(line_split_tmp1, line_split_tmp2, MAX_LINE_SIZE);
			fprintf(fp_log, "split=%s\n", line_split_tmp2);
			std::string str_utf8 = line_split_tmp2;

			// UTF8 ת�� GBK
			string str_gbk = m_utf8_tool.UTF8ToGBK(str_utf8);
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_gbk.c_str());

			
			//"�»���/nt ����/ns ʮ����/t ��ʮ����/t ��/n   ����/n ��ΰ/nr "
			len = m_tts.line2short_array(line_split_tmp1, buff_tmp, WAV_SIZE);
			if (len < 0)
			{ 
				fprintf(fp_log, "line2short_array failed !: %s\n", line_split_tmp1);
				return len; 
			}

			// �� buff_tmp������Ϊ ret�� ������β��������
			int left = 0;
			int right = len - 1;
			silence_pro(buff_tmp, len, left, right, 400);
			len = right - left;  // ���ٸ�short 

			if (buff_size < len_write+len)
			{
				fprintf(fp_log, "buff_len to small! str= %s\n", line_split_tmp1);
				return len_write;
			}

			// copy ������buff ����������
			int sss = sizeof(short);
			memcpy(buff + len_write, buff_tmp + left, len*sss);
			
			len_write += len;

			fflush(fp_log);
		}

	}


	if (buff_tmp){ delete buff_tmp; buff_tmp = NULL; }

	fclose(fp_test_out);

	return len_write;

}
/*
int TTS_ALL::tts(const char *input, short *buff, int buff_size)
{

	int ret = 0;
	int len = 0;
	int len_write = 0;  // д��buff���ܳ���
	char *delim = "\n";
	short *buff_tmp = buff;
	char *tmp;
	char lines[MAX_LINE_SIZE];
	char input_new[MAX_LINE_SIZE];
	char * input_new_tmp = input_new;

	char line_split_tmp1[MAX_LINE_SIZE];
	char line_split_tmp2[MAX_LINE_SIZE];
	std::string str_1;
	std::string str_2;
	fprintf(fp_log, "input_char*:%s\n", input);

	FILE *fp_test_out = fopen("temp.txt", "w");
	if (fp_test_out == NULL){ return -1; }
	fprintf(fp_test_out, "%s", input);
	fclose(fp_test_out);
	fp_test_out = fopen("temp.txt", "r");



	while ((tmp = fgets(lines, MAX_LINE_SIZE, fp_test_out)) != NULL)
	{
		str_1 = lines;

		// TN ����
		fprintf(fp_log, "=split_str1:%s\n", str_1.c_str());
		str_2 = m_tn_engine.ProcessText(str_1);
		fprintf(fp_log, "tn:%s\n", str_2.c_str());

		// �ַ�������  �Ͼ䣡 �ڲ����̾��Ӻϲ��ɳ����� ��
		// long ���̾��Ӻϲ�  small ��������
		std::vector<std::string> vec_out;

		// ʹ��small��ʱ��  �̾���  kehu-165����֡��̾��ӵĿ�ͷ���ֶ�ʧ����
		//ret = this->m_pp.pre_pro_small(str_2, vec_out);

		ret = this->m_pp.pre_pro_long(str_2, vec_out, 50); // 17���� 
		//if (ret < 0){ return -1; }

		// ÿһ���ַ��� �����������ַ�
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{

			str_1 = *it_vec;
			fprintf(fp_log, "pre_pro:%s\n", str_1.c_str());

			// ֱ��ʹ�� UTF8 
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_1.c_str());
			str_trim(line_split_tmp1);

			// add  ����ִʺʹ��Ա�ע��  �ر𳤵Ĵ� һ�پ�ʮ�� �� ��Ԫ���л����񹲺͹�  �� 
			SplitGBK(line_split_tmp1, line_split_tmp2, MAX_LINE_SIZE);
			fprintf(fp_log, "split=%s\n", line_split_tmp2);
			std::string str_utf8 = line_split_tmp2;

			// UTF8 ת�� GBK
			string str_gbk = m_utf8_tool.UTF8ToGBK(str_utf8);
			_snprintf(line_split_tmp1, MAX_LINE_SIZE, "%s", str_gbk.c_str());


			//"�»���/nt ����/ns ʮ����/t ��ʮ����/t ��/n   ����/n ��ΰ/nr "
			ret = m_tts.line2short_array(line_split_tmp1, buff_tmp, buff_size - len_write);
			if (ret < 0)
			{
				fprintf(fp_log, "line2short_array failed !: %s\n", line_split_tmp1);
				return ret;
			}
			buff_tmp += ret;
			len_write += ret;

			fflush(fp_log);
		}

	}



	fclose(fp_test_out);

	return len_write;

}
*/

TTS_ALL::TTS_ALL()
{

	this->fp_log = NULL;
	this->nlpir_flag = 0;
}

TTS_ALL::~TTS_ALL()
{
	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}

	if (this->nlpir_flag == 1)
	{
		delete_nlpir();
		this->nlpir_flag = 0;
	}
}


