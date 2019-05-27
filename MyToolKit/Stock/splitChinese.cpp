
#include <vector>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std; 

#include "friso.h"

friso_t		g_friso;
friso_config_t g_config;

bool friso_init()
{
	g_friso = friso_new();
	g_config = friso_new_config();

	if ( friso_init_from_ifile(g_friso, g_config, "friso.ini") != 1 ) {
		return false;
	}
	return true;
}

void friso_uninit()
{
	friso_free_config(g_config);
	friso_free(g_friso);
}

bool friso_split_chinese(string sentence, vector<string> & vecWord)
{
	vecWord.clear();

	friso_task_t task;
	task = friso_new_task();

	friso_set_text( task, (char *)sentence.c_str() );
	
	while ( ( g_config->next_token( g_friso, g_config, task ) ) != NULL ) 
	{
		string strWord = task->token->word;
		unsigned char uType = task->token->type;
		int kkk = 1;
		vecWord.insert(vecWord.end(), strWord);
	}

	friso_free_task( task );
	return true;
}