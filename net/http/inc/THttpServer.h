// $Id$
// Author: Sergey Linev   21/12/2013

/*************************************************************************
 * Copyright (C) 1995-2013, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_THttpServer
#define ROOT_THttpServer

#include "TObject.h"

#include "TList.h"

#include "TNamed.h"

#include "THttpCallArg.h"

#include <mutex>
#include <map>
#include <string>

class THttpEngine;
class THttpTimer;
class TRootSniffer;

class THttpServer : public TNamed {

protected:
   TList fEngines;                      ///<! engines which runs http server
   THttpTimer *fTimer{nullptr};         ///<! timer used to access main thread
   TRootSniffer *fSniffer{nullptr}; ///<! sniffer provides access to ROOT objects hierarchy
   Bool_t fTerminated{kFALSE};          ///<! termination flag, disables all requests processing
   Long_t fMainThrdId{0};               ///<! id of the main ROOT process

   TString fJSROOTSYS;       ///<! location of local JSROOT files
   TString fTopName{"ROOT"}; ///<! name of top folder, default - "ROOT"
   TString fJSROOT;          ///<! location of external JSROOT files

   std::map<std::string, std::string> fLocations; ///<! list of local directories, which could be accessed via server

   TString fDefaultPage;     ///<! file name for default page name
   TString fDefaultPageCont; ///<! content of the file content
   TString fDrawPage;        ///<! file name for drawing of single element
   TString fDrawPageCont;    ///<! content of draw page
   TString fCors;            ///<! CORS: sets Access-Control-Allow-Origin header for ProcessRequest responses

   std::mutex fMutex; ///<! mutex to protect list with arguments
   TList fCallArgs;   ///<! submitted arguments

   /** Function called for every processed request */
   virtual void ProcessRequest(THttpCallArg *arg);

   static Bool_t VerifyFilePath(const char *fname);

public:
   THttpServer(const char *engine = "civetweb:8080");
   virtual ~THttpServer();

   Bool_t CreateEngine(const char *engine);

   Bool_t IsAnyEngine() const { return fEngines.GetSize() > 0; }

   /** returns pointer on objects sniffer */
   TRootSniffer *GetSniffer() const { return fSniffer; }

   void SetSniffer(TRootSniffer *sniff);

   Bool_t IsReadOnly() const;

   void SetReadOnly(Bool_t readonly);

   /** set termination flag, no any further requests will be processed */
   void SetTerminate();

   /** returns kTRUE, if server was terminated */
   Bool_t IsTerminated() const { return fTerminated; }

   /** Enable CORS header to ProcessRequests() responses
    * Specified location (typically "*") add as "Access-Control-Allow-Origin" header */
   void SetCors(const char *domain = "*") { fCors = domain; }

   /** Returns kTRUE if CORS was configured */
   Bool_t IsCors() const { return fCors.Length() > 0; }

   /** Returns specified CORS domain */
   const char *GetCors() const { return fCors.Data(); }

   /** set name of top item in objects hierarchy */
   void SetTopName(const char *top) { fTopName = top; }

   /** returns name of top item in objects hierarchy */
   const char *GetTopName() const { return fTopName.Data(); }

   void SetJSROOT(const char *location);

   void AddLocation(const char *prefix, const char *path);

   void SetDefaultPage(const char *filename);

   void SetDrawPage(const char *filename);

   void SetTimer(Long_t milliSec = 100, Bool_t mode = kTRUE);

   /** Check if file is requested, thread safe */
   Bool_t IsFileRequested(const char *uri, TString &res) const;

   /** Execute HTTP request */
   Bool_t ExecuteHttp(THttpCallArg *arg);

   /** Submit HTTP request */
   Bool_t SubmitHttp(THttpCallArg *arg, Bool_t can_run_immediately = kFALSE, Bool_t ownership = kFALSE);

   /** Process submitted requests, must be called from main thread */
   void ProcessRequests();

   /** Register object in subfolder */
   Bool_t Register(const char *subfolder, TObject *obj);

   /** Unregister object */
   Bool_t Unregister(TObject *obj);

   /** Restrict access to specified object */
   void Restrict(const char *path, const char *options);

   Bool_t RegisterCommand(const char *cmdname, const char *method, const char *icon = 0);

   Bool_t Hide(const char *fullname, Bool_t hide = kTRUE);

   Bool_t SetIcon(const char *fullname, const char *iconname);

   Bool_t CreateItem(const char *fullname, const char *title);

   Bool_t SetItemField(const char *fullname, const char *name, const char *value);

   const char *GetItemField(const char *fullname, const char *name);

   /** Guess mime type base on file extension */
   static const char *GetMimeType(const char *path);

   /** Reads content of file from the disk */
   static char *ReadFileContent(const char *filename, Int_t &len);

   ClassDef(THttpServer, 0) // HTTP server for ROOT analysis
};

#endif
