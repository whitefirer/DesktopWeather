#include "StdAfx.h"
#pragma once
#include <UIlib.h>
#include "urlcoding.h"
#include "curl/curl.h"
#include "json/json.h"
#pragma comment(lib, "libcurl_imp.lib")
using namespace DuiLib;
using namespace std;
#include<iostream>

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#pragma comment(lib, "lib_json_d.lib")
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#pragma comment(lib, "lib_json.lib")
#endif

class CWeatherFrameWnd : public WindowImplBase
{
public:
	Json::Value m_weatherDict;
	Json::Value m_weatherPicPath;
	HANDLE m_hMutex;
	CWeatherFrameWnd()
	{
		m_hMutex = CreateMutex(NULL,FALSE,_T("xiamiweather"));

		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{ //如果已经存在同名的Mutex会得到这个错误.
			CloseHandle(m_hMutex);
			exit(0);
		}
	}

	~CWeatherFrameWnd()
	{
		CloseHandle(m_hMutex);
	}

	void initWeatherDict()
	{
		 m_weatherPicPath["big"] = "skin\\weather\\bigicon\\";
		 m_weatherPicPath["small"] = "skin\\weather\\icon\\";
		//m_weatherDict["晴"] = "1.png";
		m_weatherDict["晴白天"] = "1.png";
		m_weatherDict["晴晚上"] = "2.png";
		//m_weatherDict["阴"] = "3.png";
		m_weatherDict["阴白天"] = "3.png";
		m_weatherDict["阴晚上"] = "3.png";
		//m_weatherDict["多云"] = "5.png";
		m_weatherDict["多云白天"] = "5.png";
		m_weatherDict["多云晚上"] = "6.png";
		m_weatherDict["阵雨"] = "10.png";//
		m_weatherDict["雷阵雨"] = "15.png";
		m_weatherDict["雷阵雨伴有冰雹"] = "16.png";
		m_weatherDict["雨夹雪"] = "14.png";
		m_weatherDict["小雨"] = "8.png";
		m_weatherDict["中雨"] = "9.png";
		m_weatherDict["大雨"] = "10.png";
		m_weatherDict["暴雨"] = "11.png";
		m_weatherDict["大暴雨"] = "12.png";
		m_weatherDict["特大暴雨"] = "13.png";
		m_weatherDict["阵雪"]= "18.png";//
		m_weatherDict["小雪"] = "17.png";
		m_weatherDict["中雪"] = "18.png";
		m_weatherDict["大雪"] = "19.png";
		m_weatherDict["暴雪"] = "20.png";
		m_weatherDict["雾"] = "7.png";//
		m_weatherDict["冻雨"] = "14.png";//
		m_weatherDict["沙尘暴"] = "23.png";
		//m_weatherDict["小雨转中雨"] = "8.png";
		//m_weatherDict["中雨转大雨"] = "9.png";
		//m_weatherDict["大雨转暴雨"] = "10.png";
		//m_weatherDict["暴雨转大暴雨"] = "11.png";
		//m_weatherDict["大暴雨转特大暴雨"] = "12.png";
		//m_weatherDict["小雪转中雪"] = "17.png";
		//m_weatherDict["中雪转大雪"] = "18.png";
		//m_weatherDict["大雪转暴雪"] = "19.png";
		m_weatherDict["浮尘"] = "21.png";
		m_weatherDict["扬沙"] = "22.png";
		m_weatherDict["强沙尘暴"] = "24.png";
		m_weatherDict["霾"] = "7.png";//这些可以改为从配置文件里读
		
	}
	
    virtual LPCTSTR    GetWindowClassName() const   {   return _T("XiamiWeatherFrame");  }
    virtual CDuiString GetSkinFile()                {   return _T("ui.xml");  }
    virtual CDuiString GetSkinFolder()              {   return _T("");  }

	void UpdateWeather(Json::Value weatherinfo)
	{
		const Json::Value results = weatherinfo["results"];
		const Json::Value weather_data = results[0]["weather_data"];
		SetCity(results[0]["currentCity"].asCString());
		SetWeather(weather_data[0]["weather"].asCString());
		SetTemp(weather_data[0]["temperature"].asCString());
		SetWind(weather_data[0]["wind"].asCString());
		SetDate(weather_data[0]["date"].asCString());
		SetPM25(results[0]["pm25"].asCString());
		for(int i=1; i<4; i++)
		{
			SetOtherDayWeather(weather_data[i], i);
		}
	}

	void SetCity(const char* citytext)
	{   
		CControlUI* cityctrl = m_PaintManager.FindControl(_T("City"));
		if(cityctrl)
		{
			cityctrl->SetText(citytext);
		}
	}

	void SetWeatherPic(const char* weatherpic)
	{
		CControlUI* weatherPicCtrl = m_PaintManager.FindControl(_T("WeatherPic"));
		if(weatherPicCtrl)
		{
			weatherPicCtrl->SetBkImage(weatherpic);
		}
	}

	void SetWind(const char* text)
	{
		CControlUI* tempctrl = m_PaintManager.FindControl(_T("Wind"));
		if(tempctrl)
		{
			tempctrl->SetText(text);
		}
	}

	void SetDate(const char* text)
	{
		 
		CControlUI* tempctrl = m_PaintManager.FindControl(_T("curtemp"));
		if(tempctrl)
		{
			tempctrl->SetText(text);
		}
		tempctrl = m_PaintManager.FindControl(_T("date"));
		if(tempctrl)
		{
			string text1 =  strtok((char*)text, " ");
			tempctrl->SetText(text1.c_str());
		}	
	}

	void SetPM25(const char* text)
	{
		CControlUI* tempctrl = m_PaintManager.FindControl(_T("pm25"));
		if(tempctrl)
		{
			int pm = 0;
			string pmtext = text;
			sscanf(text, "%d", &pm);
			if(pm < 51)
			{
				pmtext += "优";
				tempctrl->SetBkColor(0xFF348E25);
			}
			else if(pm < 101)
			{
				pmtext += "良";
				tempctrl->SetBkColor(0xFFAFDB00);
			}
			else if(pm < 151)
			{
				pmtext += "轻度污染";
				tempctrl->SetBkColor(0xFFFFA07A);
			}
			else if(pm < 201)
			{
				pmtext += "中度污染";
				tempctrl->SetBkColor(0xFFAF1010);
			}
			else if(pm < 301)
			{
				pmtext += "重度污染";
				tempctrl->SetBkColor(0xFFEE00EE);
			}
			else
			{
				pmtext += "严重污染";
				tempctrl->SetBkColor(0xFF8B1A1A);
			}

		
			tempctrl->SetText(pmtext.c_str());
		}
	}

	string GetWeatherPicPath(const char* weathertext, bool flag=false)
	{
		string weather = weathertext;
		string picpath = m_weatherDict.get(weather, "").asString();
		if ( "" == picpath)//不能找到
		{
			//先试下能不能不切出转字来，能找到的转字已经可以直接设了
			weather = (char*)wcstok((wchar_t*)weather.c_str(), (const wchar_t*)"转");
			picpath = m_weatherDict.get(weather, "").asString();
			if ( "" == picpath)
			{
				//还是找不到那就加上时间
				SYSTEMTIME sys; 
				GetLocalTime(&sys);
				if(sys.wHour > 18 || sys.wHour < 7)
					weather += "晚上";
				else if(sys.wHour > 6)
					weather += "白天";
				picpath = m_weatherDict.get(weather, "").asString();
			}
		}
		else//能找到那也加上下时间看下
		{		
				SYSTEMTIME sys; 
				GetLocalTime(&sys);
				if(sys.wHour > 18 || sys.wHour < 7 )
					weather += "晚上";
				else if(sys.wHour > 6)
					weather += "白天";
				picpath = m_weatherDict.get(weather, "").asString();
				if ( "" == picpath)
				{
					picpath = m_weatherDict.get(weathertext, "").asString();
				}
		}
		string pictype = "big";
		if(true == flag)
		{
			pictype = "small";
		}
		picpath = m_weatherPicPath.get(pictype, "").asString() + picpath;
		//cout << "picpath：" << picpath << endl;

		return picpath;
	}

	void SetWeather(const char* weathertext)
	{
		CControlUI* tempctrl = m_PaintManager.FindControl(_T("Weather"));
		if(tempctrl)
		{
			tempctrl->SetText(weathertext);
		}
		 
		string picpath = GetWeatherPicPath(weathertext);
		SetWeatherPic(picpath.c_str());
	}

	void SetTemp(const char* temptext)
	{
		CControlUI* tempctrl = m_PaintManager.FindControl(_T("temp"));
		if(tempctrl)
		{
			tempctrl->SetText(temptext);
		}
	}

	void SetOtherDayWeather(Json::Value weatherinfo, int i)
	{
		char buffer[255];
		memset(buffer, 0, 255);
		sprintf(buffer, "Wind%d", i);
		string ctrlname = buffer;
		CControlUI* tempctrl = m_PaintManager.FindControl(ctrlname.c_str());
		if(tempctrl)
		{
			tempctrl->SetText(weatherinfo["wind"].asCString());
		}

		memset(buffer, 0, 255);
		sprintf(buffer, "Weather%d", i);
		ctrlname = buffer;
		tempctrl = m_PaintManager.FindControl(ctrlname.c_str());
		if(tempctrl)
		{
			tempctrl->SetText(weatherinfo["weather"].asCString());
		}

		memset(buffer, 0, 255);
		sprintf(buffer, "temp%d", i);
		ctrlname = buffer;
		tempctrl = m_PaintManager.FindControl(ctrlname.c_str());
		if(tempctrl)
		{
			tempctrl->SetText(weatherinfo["temperature"].asCString());
		}

		memset(buffer, 0, 255);
		sprintf(buffer, "date%d", i);
		ctrlname = buffer;
		tempctrl = m_PaintManager.FindControl(ctrlname.c_str());
		if(tempctrl)
		{
			tempctrl->SetText(weatherinfo["date"].asCString());
		}

		memset(buffer, 0, 255);
		sprintf(buffer, "WeatherPic%d", i);
		ctrlname = buffer;
		tempctrl = m_PaintManager.FindControl(ctrlname.c_str());
		if(tempctrl)
		{
			string picpath = GetWeatherPicPath(weatherinfo["weather"].asCString(), true);
			tempctrl->SetBkImage(picpath.c_str());
		}
	}

	LRESULT HandleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)// 屏蔽双击标题栏最大化
    {
		if(WM_NCLBUTTONDBLCLK != uMsg &&  WM_SIZE !=uMsg)
		{
			return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
		}
      
   return 0;
}
};

/**
 * 一旦curl接收到数据，就会调用此回调函数
 * buffer:数据缓冲区指针
 * size:调试阶段总是发现为1
 * nmemb:(memory block)代表此次接受的内存块的长度
 * writerData:用户自定义的一个参数
 */
size_t write_data(char* data, size_t size, size_t nmemb, string* writerData)
{
    if (writerData == NULL)
	{
        return 0;
	}
	int len = size*nmemb;
	writerData->append(data, len);
	return len;
}

string UrlRequestData(string urlstr)
{
	curl_global_init(CURL_GLOBAL_ALL); // 首先全局初始化CURL
    CURL* curl = curl_easy_init(); // 初始化CURL句柄

    if (NULL == curl)
    {
		::MessageBox(NULL, "请查看网络是否就绪","NULL",1);
        return "";
    }
	struct curl_slist *chunk = NULL;  
    if (curl)  
    {  
        cout << curl_version() << endl;  
        chunk = curl_slist_append(chunk, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");  
        chunk = curl_slist_append(chunk, "Accept-Charset:GBK,utf-8;q=0.7,*;q=0.3");  
        chunk = curl_slist_append(chunk, "Accept-Language:zh-TW,zh;q=0.8,en-US;q=0.6,en;q=0.4,zh-CN;q=0.2");  
        //chunk = curl_slist_append(chunk, "Connection:keep-alive");    
        chunk = curl_slist_append(chunk, "Accept-Encoding:gunzip,defate");  
        chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31");  
        chunk = curl_slist_append(chunk, "Expect:");
	}
    string buffer;
    // 设置目标URL
	

	curl_easy_setopt(curl, CURLOPT_URL, urlstr.c_str());
    //curl_easy_setopt(curl, CURLOPT_URL, "http://api.map.baidu.com/telematics/v3/weather?location=%E5%8C%97%E4%BA%AC&output=json&ak=EAdbf1bd711caf257fb60f507f9b8e4c");
    // 设置接收到HTTP服务器的数据时调用的回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    // 设置自定义参数(回调函数的第四个参数)
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	// 设置header
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	//curl_easy_setopt(curl,CURLOPT_USERAGENT,_T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; .NET CLR 1.1.4322)Accept-Encoding: gzip")); 
    // 执行一次URL请求
    CURLcode res = curl_easy_perform(curl);
    // 清理干净
    curl_easy_cleanup(curl);
	//Sleep(1000);

	if (buffer.empty())
    {
		//::MessageBox(NULL, "网络连接失败","提示",1);
		cout << "网络连接失败" << endl;
		return "";
	}
	
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)buffer.c_str(), -1, NULL,0); 
	unsigned short* wszGBK = new unsigned short[len+1];       
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)buffer.c_str(), -1, (LPWSTR)wszGBK, len); 
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL); 				
	buffer = szGBK;
	
	//std::cout << buffer << std::endl;
	return buffer;
}


void WriteDataToFile(char* filepath, const char* buffer)//可以在这里加一层加密
{
	HANDLE hFile;
	DWORD nBytes;

	hFile = ::CreateFile(filepath,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		::WriteFile(hFile,buffer, strlen(buffer), &nBytes, NULL);
		CloseHandle(hFile);
	}
}

string ReadDataFromFile(char* filename)
{
	HANDLE pfile;
	static string bufferstr = "";

	pfile = ::CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);//用这个函数比OpenFile好
	if(pfile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,"打开配置文件失败","提示",MB_OK);
		CloseHandle(pfile);//一定注意在函数退出之前对句柄进行释放。
		exit(0);
		//return bufferstr;
	}
	DWORD filesize = GetFileSize(pfile,NULL);
	char*buffer = new char[filesize+1];
	DWORD readsize;

	ReadFile(pfile,buffer,filesize,&readsize,NULL);
	buffer[filesize] = 0;
	bufferstr = buffer;
	delete[]buffer;
	CloseHandle(pfile);
	//std::cout << bufferstr << std::endl;
	return bufferstr;
}
string GetCityInfoByIP_Sina()//新浪接口
{
	string retstr = "";
	string buffer = UrlRequestData("http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=js");
	if (buffer == "")
	{
		buffer = ReadDataFromFile(_T("cityinfo-sina.json"));
		if(buffer == "")
			MessageBox(NULL,"打开配置文件失败","提示",MB_OK);
			return retstr;
	}

    static Json::Value root;
    Json::Reader reader;
	bool parsingSuccessful = reader.parse(buffer.replace(0,strlen("var remote_ip_info = "),1,' '), root);
    if(!parsingSuccessful)
    {
		::MessageBox(NULL, "数据解析出错","提示",1);
		return retstr;
	}

	static const Json::Value status = root["ret"];
	if (1 != status.asInt())
	{
		::MessageBox(NULL, "返回出错","提示",1);
		return retstr;
	}

	WriteDataToFile(_T("cityinfo-sina.json"), buffer.c_str());
	return root["city"].asString();
}

string GetCityInfoByIP()//百度接口
{
	string retstr = "";
	string buffer = UrlRequestData("http://api.map.baidu.com/location/ip?ak=EAdbf1bd711caf257fb60f507f9b8e4c");
	if (buffer == "")
	{
		buffer = ReadDataFromFile(_T("cityinfo.json"));
		if(buffer == "")
			return retstr;
	}

    static Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(buffer, root);
    if(!parsingSuccessful)
    {
		::MessageBox(NULL, "数据解析出错","提示",1);
		return retstr;
	}

	static const Json::Value status = root["status"];
	if (0 != status.asInt())
	{
		::MessageBox(NULL, "APP SN校验出错","提示",1);
		return retstr;
	}

	WriteDataToFile(_T("cityinfo.json"), buffer.c_str());
	return root["content"]["address_detail"]["city"].asString();
}

Json::Value GetWeatherInfoByCity(const char * citystr)
{
    strCoding sC;
	string urlstr = "http://api.map.baidu.com/telematics/v3/weather?output=json&ak=EAdbf1bd711caf257fb60f507f9b8e4c&location=";
	string urlcitystr = sC.UrlUTF8((char*)citystr);
	urlstr += urlcitystr;

	cout << "RequestUrl: " << urlstr << endl;

	string buffer = UrlRequestData(urlstr);
	
	if (buffer == "")
	{
		buffer = ReadDataFromFile(_T("weatherinfo.json"));
		if(buffer == "")
			return NULL;
	}
    static Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(buffer, root);
    if(!parsingSuccessful)
    {
		::MessageBox(NULL, "数据解析出错","提示",1);
		return NULL;
	}

	static const Json::Value status = root["status"];
	if (true == status.isInt())
	{
		::MessageBox(NULL, "APP SN校验出错","提示",1);
		return NULL;
	}

	static const Json::Value errorcode = root["error"];
	if (0 != errorcode.asInt())
	{
		buffer = ReadDataFromFile(_T("weatherinfo.json"));
		if(buffer == "")
			return NULL;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(buffer, root);
		if(!parsingSuccessful)
		{
			::MessageBox(NULL, "数据解析出错","提示",1);
			return NULL;
		}

		static const Json::Value status = root["status"];
		if (true == status.isInt())
		{
			::MessageBox(NULL, "APP SN校验出错","提示",1);
			return NULL;
		}
	}
	
	WriteDataToFile(_T("weatherinfo.json"), buffer.c_str());

    return root;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	#ifdef _DEBUG
		AllocConsole();
		freopen( "CONOUT$", "w+t", stdout );// 申请写
		freopen( "CONIN$", "r+t", stdin );// 申请读
	#else	
		CPaintManagerUI::SetResourceZip(_T("skin.zip"));
		CPaintManagerUI::ReloadSkin();
	#endif

	cout << "By: whitefirer@gmail.com" << endl;
	//static const Json::Value weatherinfo = GetWeatherInfoByCity("广州");
	string city = GetCityInfoByIP_Sina();
	if(city == "")
	{
		city == GetCityInfoByIP();
	}
	static const Json::Value weatherinfo = GetWeatherInfoByCity(city.c_str());
	if(weatherinfo == NULL)
	{
		return 0;
	}
	cout << weatherinfo.toStyledString() << endl;

    CPaintManagerUI::SetInstance(hInstance);

    CWeatherFrameWnd WeatherFrame;
	WeatherFrame.initWeatherDict();
	WeatherFrame.Create(NULL, _T("虾米天气"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
    //duiFrame.CenterWindow();
	WeatherFrame.SetPosAtCornerWindow(2, 2, 2);
	WeatherFrame.UpdateWeather(weatherinfo);
    WeatherFrame.ShowModal();
	
    return 0;
}