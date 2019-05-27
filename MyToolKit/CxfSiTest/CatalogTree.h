#pragma once

class CCatalogTree
{
public:
	CCatalogTree(string username, string realm, string contact);
	~CCatalogTree(void);

	int SaveOrUpdateItem(sipd_catalog_item * item);

	string GetNextHopUri(string device_id);
	sipd_catalog_item * FindCatalogItem(string device_id);

public:
	time_t m_last_update;

	map<string, sipd_catalog_item *>  m_item;
	string m_username;
	string m_contact;
	string m_realm;

};
