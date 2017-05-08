/*
* This program could get the text's semantic
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "msp_cmn.h"
#include "msp_errors.h"
#include "cJSON.h"

#define RESULTTEXT  "semantic.txt" //语义结果文本

/*

text 待转换的文本
result 转换的json结果

*/
int get_meaning (char* text,char* result,unsigned int str_len){
	int 		 ret			= 0;
	
	result = MSPSearch("nlp_version=2.0",text,&str_len,&ret);
	if(MSP_SUCCESS !=ret)
	{
		printf("MSPSearch failed ,error code is:%d\n",ret);
		return -1;
	}
	return 1;	
}

int json_to_code(char* json)
{
  int 		   device         = 0; //设备码
  int  		   operation	  = 0; //操作码
  int 		   attrbute		  = 0; //属性码			
//开始解析cjson文件
  cJSON  *root = cJSON_Parse(json);
//get rc value
  int rc = cJSON_GetObjectItem(root,"rc")->valueint;
  if(rc != 0)
  {
	printf("the rc is %d\n",rc);
	cJSON_Delete(root);
	return -1;
  }
  else
  {
	printf("result is right\n");
	char* service= cJSON_GetObjectItem(root,"service")->valuestring;
	printf("service is %s\n",service);
	if(strcmp(service,"light_smartHome") == 0)//如果为智能家居灯的控制命令
	{
		device = 12;
		attrbute = 0;
		char* slots = cJSON_GetObjectItem(cJSON_GetObjectItem(root,"semantic"),"slots");
		char* attr = cJSON_GetObjectItem(slots,"attr")->valuestring;
		printf("attr is %s\n",attr);
		if(strcmp(attr,"开关") == 0)
		{	
			char* attrValue = cJSON_GetObjectItem(slots,"attrValue")->valuestring;
			if(strcmp(attrValue,"开")==0) operation = 1;
			else operation = 2;	
		}
		else if(strcmp(attr,"亮度") == 0)
		{	
			char* direct = cJSON_GetObjectItem(cJSON_GetObjectItem(slots,"attrValue"),"direct")->valuestring;
			if(strcmp(direct,"+") ==0) operation = 3;
			else operation = 4;	
		}
		else operation = 0;
		
		if(strstr(json,"location") != NULL)
			{
				char* room = cJSON_GetObjectItem(cJSON_GetObjectItem(slots,"location"),"room")->valuestring;
				if(strcmp(room,"客厅") == 0) attrbute = 1;
				else if(strcmp(room,"厨房") == 0) attrbute = 2;
				else if(strcmp(room,"卧室") == 0) attrbute = 3;
				else if(strcmp(room,"侧所") == 0) attrbute = 4;
			}
		
	}
  }
  int sendvalue = device*100+operation*10 +attrbute;
  cJSON_Delete(root);
  return sendvalue;
	
}
int main(int argc,char* argv[])
{
  const char*  login_params   = "appid = 582199a5";   // 登录参数，appid与msc库绑定,请勿随意改动
  int          ret            = MSP_SUCCESS;  //返回值
  FILE*        fp_rd          = NULL;   //读取的文件
  FILE*        fp_wr          = NULL;   //写入的文件
  long         txtSize        = 0;  //文件尺寸
  char*        text           = NULL;   //文件缓存区
  long         read_size      = 0;    //读取的大小
  unsigned int str_len        = 0;
  const char*  rec_text       = NULL;
  int 		   device         = 0; //设备码
  int  		   operation	  = 0; //操作码
  int 		   attrbute		  = 0; //属性码
  int 		   sendvalue	  = 0;

  /*用户登录*/
  ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数  
  if (MSP_SUCCESS != ret)
  {
    printf("MSPLogin failed , Error code %d.\n",ret);
    goto main_exit; //登录失败，退出登录
  }
  /*打开要转换的文件*/
  fp_rd=fopen(argv[1],"rb");
  if(NULL == fp_rd)
  {
    printf("\nopen [%s] failed! \n",argv[1]);
    goto main_exit;
  }
	  fseek(fp_rd, 0, SEEK_END);
	  txtSize = ftell(fp_rd);
	  fseek(fp_rd, 0, SEEK_SET);

	  text = (char *)malloc(txtSize+1);
	  if (NULL == text)
	  {
		printf("\nout of memory! \n");
		goto main_exit;
	  }

	  read_size = fread((void *)text,1, txtSize, fp_rd);
	  if (read_size != txtSize)
	  {
		printf("\nread [%s] error!\n", argv[1]);
		goto main_exit;
	  }
	  text[txtSize]='\0';
	  str_len = strlen(text);
	
	  /*开始语义解析*/
	  
	  printf("%d,%d\n",(int)txtSize,str_len);
	  rec_text = MSPSearch("nlp_version=2.0",text,&str_len,&ret);
	  if(MSP_SUCCESS !=ret)
	  {
		printf("MSPSearch failed ,error code is:%d\n",ret);
		goto main_exit;
	  }
	  
	  //打开写入的文件
	  /*
	  fp_wr=fopen(RESULTTEXT,"wb");
	  if(NULL == fp_wr)
	  {
		printf("\nopen [%s] failed! \n",RESULTTEXT);
		goto main_exit;
	  }

	  read_size = fwrite(rec_text,1,str_len,fp_wr);
//  fprintf(fp_wr, "%s\n",rec_text );
  if(read_size != str_len)
  {
    printf("\nwrite [%s] error!\n", RESULTTEXT);
    goto main_exit;
  }*/
  /*
//开始解析cjson文件
  cJSON  *root = cJSON_Parse(rec_text);
//get rc value
  int rc = cJSON_GetObjectItem(root,"rc")->valueint;
  if(rc != 0)
  {
	printf("the rc is %d\n",rc);
  }
  else
  {
	printf("result is right\n");
	char* service= cJSON_GetObjectItem(root,"service")->valuestring;
	printf("service is %s\n",service);
	if(strcmp(service,"light_smartHome") == 0)//如果为智能家居灯的控制命令
	{
		device = 12;
		attrbute = 0;
		char* slots = cJSON_GetObjectItem(cJSON_GetObjectItem(root,"semantic"),"slots");
		char* attr = cJSON_GetObjectItem(slots,"attr")->valuestring;
		printf("attr is %s\n",attr);
		if(strcmp(attr,"开关") == 0)
		{	
			char* attrValue = cJSON_GetObjectItem(slots,"attrValue")->valuestring;
			if(strcmp(attrValue,"开")==0) operation = 1;
			else operation = 2;	
		}
		else if(strcmp(attr,"亮度") == 0)
		{	
			char* direct = cJSON_GetObjectItem(cJSON_GetObjectItem(slots,"attrValue"),"direct")->valuestring;
			if(strcmp(direct,"+") ==0) operation = 3;
			else operation = 4;	
		}
		else operation = 0;
		
		if(strstr(rec_text,"location") != NULL)
			{
				char* room = cJSON_GetObjectItem(cJSON_GetObjectItem(slots,"location"),"room")->valuestring;
				if(strcmp(room,"客厅") == 0) attrbute = 1;
				else if(strcmp(room,"厨房") == 0) attrbute = 2;
				else if(strcmp(room,"卧室") == 0) attrbute = 3;
				else if(strcmp(room,"侧所") == 0) attrbute = 4;
				/*
				switch (*room)
				{
					case 客厅: attrbute = 1;break;
					case 厨房: attrbute = 2;break;
					case 卧室: attrbute = 3;break;
					case 侧所: attrbute = 4;break;
					default:attrbute = 0;	
				}
				
			}
		
	}
  }
  
  sendvalue = device*100+operation*10 +attrbute;
*/

 sendvalue = json_to_code(rec_text);
 
 printf ("sendvalue is %d\n",sendvalue);
 
 net_server();//开启网络服务器
  MSPLogout(); //退出登录
  return 1;

main_exit:

  if (NULL != fp_rd)
  {
    fclose(fp_rd);
    fp_rd = NULL;
  }
  if (NULL != fp_wr)
  {
    fclose(fp_wr);
    fp_wr = NULL;
  }
  if (NULL != text)
  {
    free(text);
    text = NULL;
  }
  MSPLogout(); //退出登录

  return 0;

}


