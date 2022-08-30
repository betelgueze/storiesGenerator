#ifndef STORIES_GENERATOR_HPP
#define STORIES_GENERATOR_HPP

#include <list>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFile>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <stdlib.h>
#include <time.h>

class StoriesList;
class StoryTypesList;
class VsuvkyList;

#define STORIES_TO_MERGE 100

class Node{
public:
    std::string data;
    Node * left_child;
    Node * right_child;
    std::list<std::string> m_possibilities;
public:
    Node(std::string & line, std::list<std::string>& posses)
    {
        data = line;
        left_child = NULL;
        right_child = NULL;
        m_possibilities = posses;
    }
};

class Story
{
public:
    Node * m_root;
    std::list<std::string> m_interpretationsList;
    std::string m_storyName;


    Story(const std::string & fname)
    {
        m_root = NULL;
        m_storyName = fname;
    }
    void AddItem(std::string& line, std::list<std::string> & posses)
    {
        if (m_root == NULL)
        {
            m_root = new Node(line,posses);
            return;
        }

        std::queue<Node*> q;
        q.push(m_root);

        while (!q.empty()) {
            Node* temp = q.front();
            q.pop();

            if (temp->left_child != NULL)
                q.push(temp->left_child);
            else {
                temp->left_child = new Node(line,posses);
                break;
            }

            if (temp->right_child != NULL)
                q.push(temp->right_child);
            else
            {
                temp->right_child = new Node(line,posses);
                break;
            }
        }
    }
};

inline std::string& rtrim(std::string& s, const char* t )
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t )
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t )
{
    return ltrim(rtrim(s, t), t);
}

static void _readInterpretations(std::string & data, std::list<std::string>& interpretationsList)
{
    ltrim(data,"[ ");
    rtrim(data,"] ");
    std::istringstream iss(data);
    std::vector<std::string> res((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    interpretationsList.insert(interpretationsList.end(), res.begin(), res.end());
}
static int _readInterpretationsX(std::string & data, std::list<std::string>& interpretationsList)
{
    ltrim(data,"[ ");
    rtrim(data,"] ");
    std::istringstream iss(data);
    std::vector<std::string> res((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());


    interpretationsList.insert(interpretationsList.end(), res[0]);
    return stoi(res[1]);
}

class VsuvkyBigStorage{
public:
    std::vector<std::string *> data;
    VsuvkyBigStorage(){}
};
class Vsuvka{
public:
    std::string data;
    std::list<std::string> m_interpretationsList;
    int priority;


    void AddItem(std::string & line)
    {
        this->data.append(line.c_str());
    }
};
class VsuvkyList
{
public:
    std::list<Vsuvka*> data;


    void ReadFromDirectory(){
        QDir directory("./vsuvky");
        QStringList images = directory.entryList(QStringList() << "*.txt" ,QDir::Files);
        foreach(QString filename, images) {
            QString fullPath("./vsuvky/");
            fullPath += filename;
            this->processVsuvkaFile(fullPath);
        }
    }
private:
    void processVsuvkaFile(QString& filename)
    {
        std::string data;
        std::fstream file;
        file.open(filename.toStdString().c_str(),std::ios::in);

        int temp_priority=0;
        std::list<std::string> temp_il;
        while(getline(file, data)){            
            if(data.at(0) == ';')
                continue;
            else if(data.at(0) == '[')
            {
                temp_priority = _readInterpretationsX(data,temp_il);
                continue;
            }
            Vsuvka * vs = new Vsuvka();
            vs->priority = temp_priority;
            vs->m_interpretationsList.insert(vs->m_interpretationsList.end(),temp_il.begin(),temp_il.end());
            temp_il.clear();
            vs->AddItem(data);
            this->data.push_back(vs);
        }
        file.close();
    }

};

class StoriesList
{
public:
    std::list<Story*> data;

    StoriesList(){}
    void ReadFromDirectory(){
        QDir directory("./trees");
        QStringList images = directory.entryList(QStringList() << "*.txt" ,QDir::Files);
        foreach(QString filename, images) {
            QString fullPath("./trees/");
            fullPath += filename;
            this->processStoryFile(fullPath,filename);
        }
    }
    void processStoryText(std::string & sname, std::string & data,Story * st)
    {
        st->m_storyName = sname;

        std::stringstream ss;
        ss.str (data);

        std::string line;

        while(getline(ss, line)){
            if(line.length() == 0)
                continue;
            size_t pos;
            std::list<std::string> possibilities;

            if(line.at(0) == '['){
                _readInterpretations(data,st->m_interpretationsList);
                continue;
            }
            else if((pos = line.find(std::string("["))), pos != std::string::npos)
            {
                std::string substr = line.substr(pos);
                substr = ltrim(substr,"[ ");
                substr = rtrim(substr,"] ");
                _getPossibilities(substr,possibilities);
            }
            st->AddItem(line, possibilities);
        }

        this->data.push_back(st);
    }
private:
    void processStoryFile(QString& filename,QString& fname)
    {
        std::string data;
        std::fstream file;
        file.open(filename.toStdString().c_str(),std::ios::in);
        Story * st = new Story(fname.toStdString());
        while(getline(file, data)){
            size_t pos;
            std::list<std::string> possibilities;

            if(data.at(0) == '#')
                continue;
            else if(data.at(0) == '['){
                _readInterpretations(data, st->m_interpretationsList);
                continue;
            }
            else if((pos = data.find(std::string("["))), pos != std::string::npos)
            {
                std::string substr = data.substr(pos);
                substr = ltrim(substr,"[ ");
                substr = rtrim(substr,"] ");
                _getPossibilities(substr, possibilities);
            }
            st->AddItem(data, possibilities);
        }
        file.close();

        this->data.push_back(st);
    }
public:
    static void _getPossibilities(std::string & data, std::list<std::string> & ret)
    {
        std::istringstream iss(data);
        std::vector<std::string> res ((std::istream_iterator<std::string>(iss)),
                                         std::istream_iterator<std::string>());
        ret.insert(ret.end(), res.begin(), res.end());
    }

    void GenerateOne(VsuvkyList * vl, std::string & result)
    {
        //pick random starting story
        srand (time(NULL));
        int leadingStoryIndex = rand() % this->data.size();
        this->GenerateStartingAtIndex(leadingStoryIndex,vl,result);
    }
private:
    void GenerateStartingAtIndex(int startingIndex, VsuvkyList * vl,std::string & result)
    {
        auto l_front = this->data.begin();
        std::advance(l_front, startingIndex);
        Story * st = *l_front;

        std::list<size_t> alreadyUsedStoriesIndexes;
        alreadyUsedStoriesIndexes.push_back(startingIndex);

        VsuvkyBigStorage vbs;
        _generateVsuvkyBigStorage(vl,vbs);        

        for(int i=0; i < STORIES_TO_MERGE; ++i){
            if(st == NULL)
                break;
            this->randomStoryTraversal(vbs,st,alreadyUsedStoriesIndexes,result);
            st = _pickAnotherStory(alreadyUsedStoriesIndexes);
        }
    }
    void randomStoryTraversal( VsuvkyBigStorage & vbs, Story * st, std::list<size_t> & alreadyUsedStoriesIndexes, std::string & result)
    {
        Node * currentNode = st->m_root;
        while(currentNode != NULL)
        {
            result += currentNode->data;
            result += _getRandomVsuvka(vbs);
            currentNode = _pickContinuation(currentNode);
        }
        result += "\n";
    }

    Node * _pickContinuation(Node * nd)
    {
        if(nd->left_child == NULL)
            return nd->right_child;
        if(nd->right_child == NULL)
            return nd->left_child;

        //randomly choose continuation
        int continuationIndex = rand() % 2;
        if(continuationIndex == 0)
            return nd->left_child;
        else
            return nd->right_child;
    }

    std::string _getRandomVsuvka(VsuvkyBigStorage & vbs)
    {

        std::string result = ", ";
        if(vbs.data.size() == 0)
            return result;
        int pickedVsuvkaIndex = rand() % vbs.data.size();
        result += *vbs.data[pickedVsuvkaIndex];

        result += ", ";

        return result;
    }

    void _generateVsuvkyBigStorage(VsuvkyList * vl, VsuvkyBigStorage & vbs)
    {
        for(Vsuvka * vs: vl->data)
        {
            for(int i=0; i < vs->priority; ++i)
                vbs.data.push_back(&vs->data);
        }
    }

    Story * _pickAnotherStory(std::list<size_t> & alreadyUsedStoriesIndexes)
    {
        if(alreadyUsedStoriesIndexes.size() == this->data.size())
            return NULL;

        while(true)
        {
            int nextStoryIndex = rand() % this->data.size();
            std::list<size_t>::iterator it;
            it = std::find(alreadyUsedStoriesIndexes.begin(),alreadyUsedStoriesIndexes.end(),nextStoryIndex);
            if(it != alreadyUsedStoriesIndexes.end())
            {
                alreadyUsedStoriesIndexes.push_front(nextStoryIndex);
                auto itt = this->data.begin();
                std::advance(itt, nextStoryIndex);
                return *itt;
            }
        }
    }
};

class StoryType{
public:
    std::string data;
    int numberOfOccurences;
    StoryType():data(""),numberOfOccurences(0){}
    StoryType(std::string & str):data(str),numberOfOccurences(0){}
    StoryType(std::string & str, int num):data(str),numberOfOccurences(num){}
};

static void _clearDuplicates(std::list<StoryType> & list)
{
    std::list<StoryType>::iterator it;
    for (it = list.begin(); it != list.end(); ++it){
        std::list<StoryType>::iterator itt;
        for (itt = std::next(it); itt != list.end(); ++itt){
            if((*it).data.compare((*itt).data) == 0)
            {
                ++(*it).numberOfOccurences;
                list.erase(itt);
                break;
            }
        }
    }
}

class StoryTypesList
{
public:
    std::list<StoryType> data;
    StoryTypesList(){}

    void ReadFromListOfStories(StoriesList * sl){
        std::list<Story*>::iterator it;
        for (it = sl->data.begin(); it != sl->data.end(); ++it){
            std::list<std::string>::iterator itt;
            for(itt = (*it)->m_interpretationsList.begin() ; itt !=  (*it)->m_interpretationsList.end(); ++itt)
            {
                data.insert(data.end(),StoryType((*itt),1));
            }

        }

        _clearDuplicates(this->data);
    }
    void AddUniqueStringToList(std::string & str)
    {
        for(StoryType & it: this->data)
        {
            if(str.compare(it.data) == 0)
                return;
        }
        this->data.push_back(str);
    }
    void AddUniqueStringListToList(std::list<std::string> & strlst)
    {
        for(std::string & it: strlst)
        {
            this->AddUniqueStringToList(it);
        }
    }
    void PopulateFromStoriesEndings(StoriesList * sl)
    {
        std::queue<Node*> q;
        for(Story * st: sl->data){//for each story
            //for each node that has ending possibility
            q.push(st->m_root);

            while( !q.empty() ){
                Node * currentNode = q.front();
                q.pop();
                if(currentNode->left_child != NULL)
                    q.push(currentNode->left_child);
                if(currentNode->right_child != NULL)
                    q.push(currentNode->right_child);

                this->AddUniqueStringListToList(currentNode->m_possibilities);
            }
        }
    }
    void addStory(Story * st){
        for(std::string & storyType: st->m_interpretationsList){
            bool somethingFound = false;
            for(StoryType & mainStorageType: this->data){
                if(mainStorageType.data.compare(storyType) == 0){
                    ++mainStorageType.numberOfOccurences;
                    somethingFound  = true;
                }
            }
            if(somethingFound == false){
                this->data.push_back(StoryType(storyType,1));
            }
        }

    }

    bool static StoryTypeCompare(const StoryType & t1, const StoryType & t2)
    {
        return (t1.numberOfOccurences > t2.numberOfOccurences);
    }
    void Sort(void)
    {
        this->data.sort(StoryTypeCompare);
    }
};

#else
;;;
#endif
