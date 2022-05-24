#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <atlbase.h>

#import <msxml6.dll>

using namespace std;
using namespace filesystem;

vector<string> start_menu(int);
void pack_mods(vector<string>);

int main()
{
	CoInitialize(NULL);
	const int version = 1;
	vector<string> ModsVec = start_menu(version);
	if (!ModsVec.empty())
	{
		pack_mods(ModsVec);
		cout << "���\n\n\n";
	}
	CoUninitialize();
}

vector<string> start_menu(int version)
{
	char Choice = 'y';
vector<string> ModsVec;

cout << "OPacker\n�汾��" << version << "\n����BUG˽��BվUID38112033\n--------------------\n��ʼ����[Y]�˳�[����]\n";
cin >> Choice;
if (Choice == 'y' || Choice == 'Y')
{
	path Folder("res_unpack\\banks\\OfficialMods");

	if (!exists(Folder))
	{
		cout << "Ŀ���ļ��в�����\n";
		return ModsVec;
	}
	else if (filesystem::is_empty(Folder))
	{
		cout << "Ŀ���ļ���Ϊ��\n";
		return ModsVec;
	}
	else
	{
		int ModNum = 0;
		directory_iterator ModsIt(Folder);
		string ModName;

		while (ModsIt != end(ModsIt))
		{
			if (exists(ModsIt->path() / "mod.xml"))
			{
				if (ModNum == 0)
				{
					cout << "--------------------\n";
				}
				ModName = ModsIt->path().filename().string();
				ModsVec.push_back(ModName);
				cout << ModNum << ". " << ModName << endl;
				++ModNum;
			}
			++ModsIt;
		}
		if (ModNum == 0)
		{
			cout << "δ�ҵ����õ�Officialmod\n";
		}
		else
		{
			cout << "���ҵ�" << ModNum << "�����ܿ��õ�Officialmod��ȫ������[Y]�˳�[����]\n";
		}
		return ModsVec;
	}
}
else
{
	return ModsVec;
}
}

void pack_mods(vector<string> ModsVec)
{
	char Choice = 'y';

	cin >> Choice;
	if (Choice == 'y' || Choice == 'Y')
	{
		MSXML2::IXMLDOMDocumentPtr ModXml;
		if (!SUCCEEDED(ModXml.CreateInstance(__uuidof(MSXML2::DOMDocument60))))
		{
			cout << "����xml����ʧ��\n";
			ModXml.Detach();
			return;
		}
		cout << "�Ѵ���xml����\n";
		cout << "--------------------\n";
		cout << "banks�ļ��н�������C:\\Games\\World_of_Warships\\bin��\n�������ƶ���bin\\�汾���\\res_mods�У�����readme.txt�ԣ�\n";
		for (vector<string>::iterator ModsIt = ModsVec.begin(); ModsIt != ModsVec.end(); ModsIt++)
		{
			vector<string> Comm;

			ModXml->put_preserveWhiteSpace(VARIANT_FALSE);
			if (VARIANT_FALSE == ModXml->load((_variant_t)("res_unpack\\banks\\OfficialMods\\" + *ModsIt + "\\mod.xml").c_str()))
			{
				cout << "xml�ļ�����ʧ�ܣ�ԭ��" << string(ModXml->parseError->Getreason()) << endl;
				ModXml.Detach();
				return;
			}
			cout << *ModsIt << "��������xml�ļ�\n";

			MSXML2::IXMLDOMNodeListPtr Containers, Names, Paths;
			MSXML2::IXMLDOMNodePtr Container, Path;

			Containers = ModXml->selectNodes("/AudioModification.xml/AudioModification/ExternalEvent/Container");
			Container = Containers->nextNode();
			Names = Container->selectNodes("Path/StateList/State[Name='CrewName']/Value");
			for (int i = 0; i < Names->Getlength(); i++)
			{
				Comm.push_back(string(Names->nextNode()->GetfirstChild()->Gettext()));
			}

			for (vector<string>::iterator CommName = Comm.begin(); CommName != Comm.end(); CommName++)
			{
				if (exists(path("bin\\banks\\Mods") / (*ModsIt + ' ' + *CommName)))
				{
					cout << *ModsIt + ' ' + *CommName << "���ļ����Ѵ���\n";
				}
				else if (!create_directories(path("bin\\banks\\Mods") / (*ModsIt + ' ' + *CommName)))
				{
					cout << *ModsIt + ' ' + *CommName << "�������ļ���ʧ��\n";
					continue;
				}
				else
				{
					cout << *ModsIt + ' ' + *CommName << "�������ļ��гɹ�\n";
				}

				ModXml->selectSingleNode("/AudioModification.xml/AudioModification/Name")->GetfirstChild()->Puttext((_bstr_t)(*ModsIt + ' ' + *CommName).c_str());
				Containers->reset();
				for (int i = 0; i < Containers->Getlength(); i++)
				{
					Container = Containers->nextNode();
					Paths = Container->GetchildNodes();
					Paths->reset();
					for (int j = 0; j < Paths->Getlength(); j++)
					{
						Path = Paths->nextNode();
						if (string(Path->GetnodeName()) == "Path" && Path->selectSingleNode("StateList/State[Name='CrewName']/Value")->hasChildNodes())
						{
							if (string(Path->selectSingleNode("StateList/State[Name='CrewName']/Value")->GetfirstChild()->Gettext()) != *CommName)
							{
								Container->removeChild(Path);
								j--;
							}
							else
							{
								Path->selectSingleNode("StateList")->removeChild(Path->selectSingleNode("StateList/State[Name='CrewName']"));
							}
						}
					}
				}
				if (!SUCCEEDED(ModXml->save((_variant_t)(("bin\\banks\\Mods\\" + *ModsIt + ' ' + *CommName + "\\mod.xml").c_str()))))
				{
					cout << "xml�ļ����ʧ�ܣ�ԭ��" << string(ModXml->parseError->Getreason()) << endl;
					Containers.Detach();
					Names.Detach();
					Paths.Detach();
					Container.Detach();
					Path.Detach();
					ModXml.Detach();
					return;
				}
				cout << *ModsIt + ' ' + *CommName << "�������xml�ļ�\n";
				//����һ��Paths��Path����Ȼ��Ҫ����������ָ��
				Paths = ModXml->selectNodes("/AudioModification.xml/AudioModification/ExternalEvent/Container/Path/FilesList/File/Name");
				Paths->reset();
				for (int i = 0; i < Paths->Getlength(); i++)
				{
					Path = Paths->nextNode()->GetfirstChild();
					copy_file("res_unpack\\banks\\OfficialMods\\" + *ModsIt + '\\' + string(Path->Gettext()),
						"bin\\banks\\Mods\\" + *ModsIt + ' ' + *CommName + '\\' + string(Path->Gettext()),
						copy_options::overwrite_existing);
				}
				cout << *ModsIt + ' ' + *CommName << "���Ѹ���wem�ļ�\n";
				if (VARIANT_FALSE == ModXml->load((_variant_t)("res_unpack\\banks\\OfficialMods\\" + *ModsIt + "\\mod.xml").c_str()))
				{
					cout << "xml�ļ�����ʧ�ܣ�ԭ��" << string(ModXml->parseError->Getreason()) << endl;
					Containers.Detach();
					Names.Detach();
					Paths.Detach();
					Container.Detach();
					Path.Detach();
					ModXml.Detach();
					return;
				}
				cout << *ModsIt << "��������xml�ļ�\n";
				Containers = ModXml->selectNodes("/AudioModification.xml/AudioModification/ExternalEvent/Container");
			}

			Containers.Detach();
			Names.Detach();
			Paths.Detach();
			Container.Detach();
			Path.Detach();
		}
		ModXml.Detach();
	}
}