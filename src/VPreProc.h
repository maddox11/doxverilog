// -*- C++ -*-
//*************************************************************************
//
// Copyright 2000-2013 by Wilson Snyder.  This program is free software;
// you can redistribute it and/or modify it under the terms of either the GNU
// Lesser General Public License Version 3 or the Perl Artistic License Version 2.0.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//*************************************************************************
/// \file
/// \brief Verilog::Preproc: Preprocess verilog code
///
/// Authors: Wilson Snyder
///
/// Code available from: http://www.veripool.org/verilog-perl
///
//*************************************************************************

#ifndef _VPREPROC_H_
#define _VPREPROC_H_ 1

#include <string>
#include <map>
#include <iostream>
#include "qmap.h"
#include "qcstring.h"
#include "assert.h"
using namespace std;
#include "VFileLine.h"
#include "define.h"
#include <deque>
#include <stack>
/// Generic opaque pointer to VPreProcImp implementation class.
struct VPreProcOpaque {
    virtual ~VPreProcOpaque() {}
};
class VDefine;

//**********************************************************************
// VPreProc
/// Verilog Preprocessor.
////
/// This defines a preprocessor.  Functions are virtual so users can override them.
/// After creating, call openFile(), then getline() in a loop.  The class will to the rest...

struct IncDef {
	string ss;
	string incFile;
	string incText;
	int incLine;
};

class VPreProc {
public:
    VPreProc();
    void configure(VFileLine* filelinep);
    virtual ~VPreProc();
   stack<IncDef> m_include;
	


    // STATE
private:
    int		m_keepComments;
    int		m_keepWhitespace;
    bool	m_lineDirectives;
    bool	m_pedantic;
    bool	m_synthesis;

public:
    // CONSTANTS
    enum MiscConsts {
 	DEFINE_RECURSION_LEVEL_MAX = 1000,	// How many `def substitutions before an error
 	INCLUDE_DEPTH_MAX = 500,		// How many `includes deep before an error
	STREAM_DEPTH_LEVEL_MAX = 2000,		// How many streams deep (sometimes `def deep) before an error
	//					// Set more than DEFINE_RECURSION_LEVEL_MAX or INCLUDE_DEPTH_MAX
 	NEWLINES_VS_TICKLINE = 20		// Use `line in place of this many newlines
    };

    // ACCESSORS
    /// Insert given file into this point in input stream
    void openFile(string filename, VFileLine* filelinep=NULL);
    void debug(int level);	///< Set debugging level
    string getall(size_t approx_chunk);	///< Return all lines, or at least approx_chunk bytes. (Null if done.)
    string getline();		///< Return next line/lines. (Null if done.)
    bool isEof();		///< Return true on EOF.
    void insertUnreadback(string text);
    int getNextStateToken(string & buf);
    VFileLine* fileline();	///< File/Line number for last getline call

    // The default behavior is to pass all unknown `defines right through.
    // This lets the user determine how to report the errors.  It also nicely
    // allows `celldefine and such to remain in the output stream.

    // CONTROL METHODS
    // These options control how the parsing proceeds
    int  keepComments() { return m_keepComments; }
    void keepComments(int flag) { m_keepComments=flag; }	// Return comments, 0=no, 1=yes, 2=callback
    int  keepWhitespace() { return m_keepWhitespace; }
    void keepWhitespace(int flag) { m_keepWhitespace=flag; }	// Return extra whitespace
    bool lineDirectives() { return m_lineDirectives; }
    void lineDirectives(bool flag) { m_lineDirectives=flag; }	// Insert `line directives
    bool pedantic() { return m_pedantic; }
    void pedantic(bool flag) { m_pedantic=flag; }	// Obey standard; Don't substitute `error
    bool synthesis() { return m_synthesis; }
    void synthesis(bool flag) { m_synthesis=flag; }	// Ignore translate off

    // CALLBACK METHODS
    // This probably will want to be overridden for given child users of this class.
    virtual void comment(string cmt) = 0;		///< Comment detected (if keepComments==2)
    virtual string include(string filename) = 0;	///< Request a include file be processed
    virtual void define(string name, string value, string params) = 0; ///< `define without any parameters
    virtual void undef(string name) = 0;		///< Remove a definition
    virtual void undefineall() = 0;			///< Remove all non-command-line definitions
    virtual bool defExists(string name) = 0;	///< Return true if define exists
    virtual string defParams(string name) = 0;	///< Return parameter list if define exists
    virtual string defValue(string name) = 0;	///< Return value of given define (should exist)
    virtual string defSubstitute(string substitute) = 0;	///< Return value to substitute for given post-parameter value

    // UTILITIES
    void error(string msg) { fileline()->error(msg); }	///< Report a error
    void fatal(string msg) { fileline()->fatal(msg); }	///< Report a fatal error
    VPreProcOpaque* getImp() const { return m_opaquep;}
private:
    VPreProcOpaque*	m_opaquep;	///< Pointer to parser's implementation data.
};

#include "qfileinfo.h"

struct VPreProcImp;

class VerilogPreProc: public VPreProc
 {
 private:
    string getIndexString(const string& x,bool upper);
    
    static DefineDict  *g_fileDefineDict;
    static DefineDict  *g_preDefineDict;

 public:
     VerilogPreProc():VPreProc(){}
     ~VerilogPreProc(){} 
 
 
  static DefineDict  *getFileDefineDict() 
   { 
      if(g_fileDefineDict==0)
       {
       g_fileDefineDict=new DefineDict();
       g_fileDefineDict->setAutoDelete(true);
       }
       
       return g_fileDefineDict;
   } 
    
   static DefineDict  *getPreDefineDict() 
   { 
      if(g_preDefineDict==0)
       {
       g_preDefineDict=new DefineDict();
       g_preDefineDict->setAutoDelete(true);
       }
       return g_preDefineDict;
   } 
   
   VPreProcImp* getImpll() { 
	   VPreProcImp* idatap = (VPreProcImp*)(this->getImp());
       return idatap; 
   }

    void  getPredefs();
  
    string performPreprocessing(const QFileInfo & qf,bool include=false);
    
     void comment(string cmt) {}		///< Comment detected (if keepComments==2)
  
     string include(string filename) ;
         
     void define(string name, string value, string params) ;

     void undef(string name); 
    
    ///< Remove all non-command-line definitions
     void undefineall() ;
    
     bool defExists(string name); 
    
     string defParams(string name) ;
    
     string defValue(string name) ;
   
     string defSubstitute(string substitute); 
     
     void printDict();
	
	 void insertIncludeText( string & text );
      
    };

#endif // Guard
