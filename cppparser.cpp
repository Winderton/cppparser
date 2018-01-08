/*
   The MIT License (MIT)

   Copyright (c) 2014

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
   the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "cppparser.h"
#include "cppobjfactory.h"
#include "cppdom.h"

#include <fstream>

extern CppCompound* parseStream(char* stm, size_t stmSize);
extern CppObjFactory* gObjFactory = nullptr;

CppParser::CppParser(CppObjFactory* objFactory)
  : objFactory_(objFactory)
{
  if (objFactory_ == nullptr)
    objFactory_ = new CppObjFactory;
}

CppCompound* CppParser::parseFile(const char* filename)
{
  auto stm = readFile(filename);
  CppCompound* cppCompound = parseStream(stm.data(), stm.size());
  if (cppCompound == NULL)
    return cppCompound;
  cppCompound->name_ = filename;
  return cppCompound;
}

CppCompound* CppParser::parseStream(char* stm, size_t stmSize)
{
  if (stm == nullptr || stmSize == 0)
    return nullptr;
  gObjFactory = objFactory_;
  return ::parseStream(stm, stmSize);
}

CppProgram* CppParser::loadProgram(const char* szInputPath)
{
  auto program = new CppProgram;
  loadProgram(szInputPath, *program);
  return program;
}

void CppParser::loadProgram(const bfs::path& path, CppProgram& program)
{
  if (bfs::is_regular_file(path))
  {
    CppCompound* cppdom = parseFile(path.string().c_str());
    if (cppdom)
      program.addCppDom(cppdom);
  }
  else if (bfs::is_directory(path))
  {
    for (bfs::directory_iterator dirItr(path); dirItr != bfs::directory_iterator(); ++dirItr)
    {
      loadProgram(*dirItr, program);
    }
  }
}

CppParser::ByteArray CppParser::readFile(const char* filename)
{
  ByteArray contents;
  std::ifstream in(filename, std::ios::in);
  if (in)
  {
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    size += 2; // For adding last 2 nulls.
    contents.resize(size);
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], size);
    in.close();
  }
  return(contents);
}
