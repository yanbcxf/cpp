#include "StdAfx.h"
#include "CatalogTree.h"

CCatalogTree::CCatalogTree(string username, string realm, string contact)
{
	m_username = username;
	m_contact = contact;
	m_realm = realm;
	m_last_update = 0;
}

CCatalogTree::~CCatalogTree(void)
{
	map<string ,sipd_catalog_item * >::iterator it = m_item.begin();
	for(;  it!=m_item.end(); it++)
	{
		sipd_catalog_item * p = it->second;
		if(p)
			free(p); 
	}
	m_item.clear();
	
}

string CCatalogTree::GetNextHopUri(string device_id)
{
	map<string ,sipd_catalog_item * >::iterator it = m_item.find(device_id);
	if(it!= m_item.end())
	{
		return m_contact;
	}
	return "";
}

sipd_catalog_item * CCatalogTree::FindCatalogItem(string device_id)
{
	sipd_catalog_item * pItem = NULL;
	map<string ,sipd_catalog_item * >::iterator it = m_item.find(device_id);
	if(it!= m_item.end())
	{
		pItem = it->second;
	}
	return pItem;
}

int CCatalogTree::SaveOrUpdateItem(sipd_catalog_item * item)
{
	int ret ;
	string key = string(item->device_id);
	map<string ,sipd_catalog_item * >::iterator it = m_item.find(key);
	if(it!= m_item.end())
	{
		// 已存在
		if(1)
		{
			//	内容有变化，则更新
			sipd_catalog_item * p = it->second;
			memcpy(p, item, sizeof(sipd_catalog_item));
			ret = 1;
		}
		else
			ret = 0;
	}
	else
	{
		//	不存在则插入
		sipd_catalog_item * p = (sipd_catalog_item *)malloc(sizeof(sipd_catalog_item));
		if(p)
		{
			memcpy(p, item, sizeof(sipd_catalog_item));
			m_item[key] = p;
			ret = 2;
		}
		else 
			ret = -1;
			
	}
	return ret;
}