#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
 
// #define __DEBUG__
 
struct Tag
{
    std::string name;
    std::map<std::string, std::string> attributes;
    std::vector<Tag*> children;
 
    ~Tag()
    {
        for (auto& child : children)
        {
            delete child;
        }
    }
};
 
void removeInvalidChars(std::string& str)
{
    const char invalidChars[] = {'<', '>', '=', '"', ' '};
 
    for (int i = 0; i < sizeof(invalidChars) / sizeof('0'); i++)
    {
        str.erase(std::remove(str.begin(), str.end(), invalidChars[i]), str.end());
    }
}
 
std::vector<std::string> split(const std::string& str, char delim = ' ')
{
    std::vector<std::string> ret;
    
    int start = 0;
 
    for (int i = 0; i < str.size(); i++)
    {
        if (i == str.size() - 1)
        {
            ret.emplace_back(str.substr(start, i - start + 1));
        }
 
        if (str.at(i) == delim)
        {
            ret.emplace_back(str.substr(start, i - start));
            start = i + 1;
        }
    }
 
    return ret;
}
 
std::vector<std::string> prepTagLine(std::string line)
{
    std::vector<std::string> values = split(line);
 
    values.erase(std::remove_if(values.begin(), values.end(),
        [](std::string& value)
        { 
            removeInvalidChars(value);
            return !value.size();
        }
    ), values.end());
 
    return values;
}
 
std::vector<Tag*> makeTags(std::vector<std::string> allLines)
{
    // assuming input is valid
    std::vector<Tag*> ret;
 
    while (allLines.size() > 0)
    {
        Tag* tag = new Tag();
        std::vector<std::string> values = prepTagLine(allLines.at(0));
 
        // name
        tag->name = values.at(0);
 
        // attributes
        for (int i = 1; i < values.size() - 1; i += 2)
        {
            tag->attributes.insert(std::make_pair(values.at(i), values.at(i + 1)));
        }
 
        // find closing tag
        int closingTagLineIdx = 1;
        for (; closingTagLineIdx < allLines.size(); closingTagLineIdx++)
        {
            if (prepTagLine(allLines.at(closingTagLineIdx)).at(0) == "/" + tag->name)
            {
                break;
            }
        }
 
        // get content of the tag and call the function again
        if (closingTagLineIdx != 1) // if has child content
        {
            std::vector<std::string> tagContent(allLines.begin() + 1, allLines.begin() + closingTagLineIdx);
            tag->children = makeTags(tagContent);
        }
 
        allLines.erase(allLines.begin(), allLines.begin() + closingTagLineIdx + 1);
 
        // insert into ret
        ret.emplace_back(tag);
    }
 
    return ret;
}
 
#ifdef __DEBUG__
 
void printTag(const Tag* tag, const std::string& prefix = "")
{
    std::cout << prefix << tag->name << ":\n";
 
    for (const auto& attrib : tag->attributes)
    {
        std::cout << prefix << "\t" << attrib.first << "=" << attrib.second << "\n";
    }
 
    std::cout << "\n";
 
    for (const auto& child : tag->children)
    {
        printTag(child, prefix + "\t");
    }
 
    std::cout << "\n";
}
 
void printTags(const std::vector<Tag*>& tags)
{
    for (const auto& tag : tags)
    {
        printTag(tag);
    }
}
 
#endif
 
std::vector<std::string> readHtml(int linesCount)
{
    std::vector<std::string> htmlLines(linesCount);
 
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < linesCount; i++)
    {
        std::getline(std::cin, htmlLines.at(i));
    }
 
    return htmlLines;
}
 
std::vector<Tag*> makeTags(int linesCount)
{
    std::vector<std::string> htmlLines = readHtml(linesCount);
    return makeTags(htmlLines);
}
 
Tag* findTagWithName(const std::vector<Tag*>& tags, const std::string& name)
{
    auto tagIt = std::find_if(tags.begin(), tags.end(), [&name](const Tag* tag) { return tag->name == name; });
    return tagIt == tags.end() ? nullptr : *tagIt;
}
 
void showQueryResult(const std::vector<Tag*>& tags, const std::string& query)
{
    auto tagPath = split(query, '.');
    Tag* currTag = nullptr;
 
    for (int i = 0; i < tagPath.size(); i++)
    {
        std::string name = tagPath.at(i);
 
        if (i + 1 == tagPath.size())
        {
            auto tagNameAndAttrib = split(name, '~');
            name = tagNameAndAttrib.at(0);
            std::string attrib = tagNameAndAttrib.at(1);
 
            currTag = findTagWithName(currTag == nullptr ? tags : currTag->children, tagNameAndAttrib.at(0));
            if (currTag == nullptr || 
                currTag->attributes.find(attrib) == currTag->attributes.end()
                )
            {
                std::cout << "Not Found!" << std::endl;
                return;
            }
 
            std::cout << currTag->attributes.at(tagNameAndAttrib.at(1)) << std::endl;
 
            return;
        }
 
        currTag = findTagWithName(currTag == nullptr ? tags : currTag->children, name);
 
        if (currTag == nullptr)
        {
            std::cout << "Not Found!" << std::endl;
            return;
        }
    }
}
 
int main() {
    /*
    
    // TEST CASE
    std::vector<std::string> test{
        "<test value = \"helloWorld\">",
            "<child value = \"xd\" value1 = \"xdd\">",
            "</child>",
 
            "<child>",
            "</child>",
        "</test>",
 
        "<xd>",
            "<a>",
            "</a>",
        "</xd>",
    };
 
    std::vector<Tag*> tags = makeTags(test);
    #ifdef __DEBUG__
    printTags(tags);
    #endif
    */
 
    int htmlLinesCount;
    int numOfQueries;
 
    std::cin >> htmlLinesCount >> numOfQueries;
 
    std::vector<Tag*> allTags = makeTags(htmlLinesCount);
 
#ifdef __DEBUG__
    printTags(allTags);
#endif
 
    while(numOfQueries--)
    {
        std::string query;
        std::cin >> query;
 
        showQueryResult(allTags, query);
    }
 
    for(auto& tag : allTags)
    {
        delete tag;
    }
 
    return 0;
}