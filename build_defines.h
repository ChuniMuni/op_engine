#pragma once
//#define SUN_DIR_DEBUG //����� ��������� � ��������� ������
//#define WEATHER_LOGGING //����� ��������� � ���������� ������
//#define MORE_SPAM // ������ ����
//#define MORE_VERIFY_PATH_MANAGER // ��� �������� 
//#define MORE_VERIFY_PROP_COND //��� �������� �� ���������� � ��� � ����
//#define DEBUG_SCHEDULER //������� ��������
//#define DEBUG_SCHEDULER2 //������� �������� �������������� :)

#define DEBUG_RESTRICTORS // ����� ���������� ���������� � ����� ������������ --alpet restrictor_type==2
//#define VERIFY_RESTRICTORS //������ �������� ��� ������������
#define IGNORE_CRITICAL_ECONDITIONS
//#define CREATE_MDUMP //��������� ���� ������
//#define SHOW_INCORRECT_r_tgt 


//#define PATCH_INFO_PRESENT

#define ENGINE_DESCRIPTION "OP 2.1 Engine"
#define ENGINE_MINOR "0"
#define ENGINE_MAJOR "51f"

#ifdef DEBUG
	#define ENGINE_BUILD_TYPE "debug"
#else
	#define	ENGINE_BUILD_TYPE "release"
#endif

#define PATCH_DESCRIPTION " patch"
#define PATCH_MINOR "0"
#define PATCH_MAJOR "5" 

