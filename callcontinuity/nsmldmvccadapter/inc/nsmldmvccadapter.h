/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   DM Adapter class definition
*
*/



#ifndef C_NSMLDMVCCADAPTER_H
#define C_NSMLDMVCCADAPTER_H


#include <smldmadapter.h>
#include <msvapi.h>

#include "vccunittesting.h"

class CVccSPSettings;

/**
 *  The main class of the OMA PoC DM adapter. 
 *  Handles requests of fetching and updating settings sets and settings items.
 * 
 *  @lib nsmldmvccadapter.dll
 *  @since S60 ?S60_version
 */
class CNSmlDmVCCAdapter : public CSmlDmAdapter
    {
public:

  /**
   * Two-phased constructor.
   *
   * @since S60 3.2
   * @param aDmCallback The pointer to callback to return results and 
   *                    status to codes for completed commands to the 
   *                    DM framework. The interface also has 
   *                    functionality for mapping LUIDs and fetching 
   *                    from other parts of the DM Tree.
   * @return A pointer to the newly created object.
   */
    static CNSmlDmVCCAdapter* NewL( MSmlDmCallback* aDmCallback );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 3.2
     * @param aDmCallback The pointer to callback to return results and 
     *                    status to codes for completed commands to the 
     *                    DM framework. The interface also has 
     *                    functionality for mapping LUIDs and fetching 
     *                    from other parts of the DM Tree.
     * @return A pointer to the newly created object.
     */
    static CNSmlDmVCCAdapter* NewLC( MSmlDmCallback* aDmCallback );

    /**
    * Destructor.
    */
    virtual ~CNSmlDmVCCAdapter();
  
//from base class MSmlDmAdapter

    /**
    * From MSmlDmAdapter.
    * Returns current version of the DDF.
    * This function is always called after DDFStructureL.
    *
    * @since S60 3.2
    * @param aVersion DDF version of the adapter. 
    */
    void DDFVersionL( CBufBase& aVersion );

    /**
    * From MSmlDmAdapter.
    * Fills the DDF structure of the adapter. The  adapter starts to 
    * fill the data structure by calling AddChildObjectL to the root
    * object and so describes the DDF of the adapter. 
    *
    * @since S60 3.2
    * @param aDDFObject Reference to the root object. 
    */
    void DDFStructureL( MSmlDmDDFObject& aDDF );

    /**
    * From MSmlDmAdapter.
    * Creates new leaf objects, or replaces data in existing
    * leaf objects. The information about the success of the command 
    * should be returned by calling SetStatusL function of MSmlDmCallback 
    * callback interface. This makes it possible to buffer the commands. 
    * However, all the status codes for buffered commands must be returned
    * at the latest when the adapter's CompleteOutstandingCmdsL() is 
    * called.
    *
    * @since S60 3.2
    * @param aURI   URI of the object
    * @param aLUID    LUID of the object (if the adapter has earlier 
    *                   returned a LUID to the DM Module). For new 
    *                   objects, this is the LUID inherited through the
    *                   parent node.
    * @param aObject  Data of the object.
    * @param aType    MIME type of the object
    * @param aStatusRef Reference to correct command, i.e. this reference
    *           must be used when calling the SetStatusL of this 
    *                   command
    */
    void UpdateLeafObjectL( const TDesC8& aURI,
                            const TDesC8& aLUID,
                      const TDesC8& aObject,
                      const TDesC8& aType,
                      TInt aStatusRef );
  
    /**
    * From MSmlDmAdapter.
    * The function creates new leaf objects, or replaces data in existing 
    * leaf objects, in the case where data is large enough to be streamed.
    * SetStatusL function of MSmlDmCallback callback interface should be 
    * called to inform about the success of the command. This makes it 
    * possible to buffer the commands.  All the  status codes for buffered
    * commands must be returned at the latest 
    * when the CompleteOutstandingCmdsL() of adapter is called.
    *
    * @since S60 3.2
    * @param aURI   URI of the object
    * @param aLUID    LUID of the object (if the adapter has earlier 
    *                   returned a LUID to the DM Module). 
    *                   For new objects, this is the LUID inherited 
    *                   through the parent node.
    * @param aStream  Data of the object. Adapter should create write 
    *                   stream and return, when data is written to stream
    *                   by DM agent, StreamCommittedL() is called by DM 
    *                   engine
    * @param aType    MIME type of the object
    * @param aStatusRef Reference to correct command, i.e. this reference
    *           must be used when calling the SetStatusL of this
    *           command.
    */
    void UpdateLeafObjectL( const TDesC8& aURI,
                            const TDesC8& aLUID,
                            RWriteStream*& aStream,
                            const TDesC8& aType,
                            TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * Deletes an object and its child objects. The information about the 
    * success of the command should be returned by calling SetStatusL 
    * function of MSmlDmCallback callback interface.
    * This makes it possible to buffer the commands.  All the 
    * status codes for buffered commands must be returned at the latest 
    * when the CompleteOutstandingCmdsL() of adapter is called.
    *
    * @since S60 3.2
    * @param aURI   URI of the object
    * @param aLUID    LUID of the object (if the adapter have earlier 
    *                   returned LUID to the DM Module).
    * @param aStatusRef Reference to correct command, i.e. this reference 
    *                   must be used when calling the SetStatusL of this 
    *                   command.
    */
    void DeleteObjectL( const TDesC8& aURI,
                        const TDesC8& aLUID,
                        TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * Fetches the data of a leaf object. The SetStatusL should be used as
    * described in UpdateLeafObjectL(). The data is returned by using the
    * SetResultsL function of MSmlCallback callback interface, and may be
    * streamed.
    *
    * @since S60 3.2
    * @param aURI     URI of the object
    * @param aLUID      LUID of the object (if the adapter have 
    *                       earlier returned LUID to the DM Module).   
    * @param aType      MIME type of the object
    * @param aResultsRef  Reference to correct results, i.e. this 
    *                       reference must be used when returning the 
    *                       result by calling the SetResultsL.
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void FetchLeafObjectL( const TDesC8& aURI,
                           const TDesC8& aLUID,
                           const TDesC8& aType, 
                           TInt aResultsRef,
                           TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * The function fetches the size of the data of a leaf object. The size
    * is in bytes, and must reflect the number of bytes that will be 
    * transferred when the framework calls FetchLeafObjectL. The 
    * SetStatusL should be used as described in FetchLeafObjectL(). The 
    * size value is returned by using the SetResultsL function of 
    * MSmlCallback callback interface, and must be a decimal integer 
    * expressed as a string, eg. "1234". Results from this call MUST NOT 
    * be streamed.
    *
    * @since S60 3.2
    * @param aURI     URI of the object
    * @param aLUID      LUID of the object (if the adapter have 
    *                       earlier returned LUID to the DM Module).   
    * @param aType      MIME type of the object
    * @param aResultsRef  Reference to correct results, i.e. this 
    *                       reference must be used when returning the 
    *                       result by calling the SetResultsL.
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void FetchLeafObjectSizeL( const TDesC8& aURI,
                               const TDesC8& aLUID,
                               const TDesC8& aType,
                               TInt aResultsRef,
                               TInt aStatusRef );
    /**
    * From MSmlDmAdapter.
    * Fetches URI list. The adapter returns the list of URI segments under
    * the given URI be separated by slash ("/"). The URI segment names for
    * new objects must be given by the adapter. The list is returned by 
    * calling the SetResultsL function of MSmlCallback callback interface.
    * Results from this call MUST NOT be streamed.
    *
    * @since S60 3.2
    * @param aParentURI        URI of the parent object
    * @param aParentLUID       LUID of the parent object (if the
    *                  adapter have earlier returned LUID
    *                                to the DM Module).   
    * @param aPreviousURISegmentList URI list with mapping LUID 
    *                                information, which is known by DM 
    *                                engine. An adapter can use this 
    *                                information when verifying if old 
    *                                objects still exists. An adapter also
    *                                knows what objects are new to DM
    *                engine and can provide LUID mapping
    *                                for them. aPreviousURISegmentList 
    *                                parameter (see above) helps to 
    *                                recognise new objects.
    * @param aResultsRef         Reference to correct results, i.e. 
    *                                this reference must be used when 
    *                                returning the result by calling the
    *                                SetResultsL.
    * @param aStatusRef        Reference to correct command, i.e. 
    *                                this reference must be used when 
    *                                calling the SetStatusL of this 
    *                                command.
    */
    void ChildURIListL( const TDesC8& aURI,
                        const TDesC8& aLUID,
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                        TInt aResultsRef,
                        TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * The function adds node object. In some cases an implementation of 
    * the function may be empty function, if the node object does not need
    * concrete database update. Still this function may be helpful to an 
    * adapter, i.e. in passing mapping LUID of the node to DM Module. The 
    * SetStatusL should be used as described in UpdateLeafObjectL().
    *
    * @since S60 3.2
    * @param aURI     URI of the object
    * @param aParentLUID  LUID of the parent object (if the adapter have
    *             earlier returned LUID to the DM Module).   
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void AddNodeObjectL( const TDesC8& aURI,
                     const TDesC8& aParentLUID,
                     TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * The function implements execute command. The information about the 
    * success of the command should be returned by calling SetStatusL 
    * function of MSmlDmCallback callback interface. This makes it 
    * possible to buffer the commands. However, all the status codes for 
    * buffered commands must be returned at the latest when the 
    * CompleteOutstandingCmdsL() of adapter is called.
    *
    * @since S60 3.2
    * @param aURI     URI of the command
    * @param aLUID      LUID of the object (if the adapter have 
    *                       earlier returned LUID to the DM Module).   
    * @param aArgument    Argument for the command
    * @param aType      MIME type of the object 
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void ExecuteCommandL( const TDesC8& aURI,
                      const TDesC8& aLUID,
                      const TDesC8& aArgument,
                      const TDesC8& aType,
                      TInt aStatusRef );

    /**
    * From MSmlDmAdapter.
    * The function implements execute command. The information about the
    * success of the command should be returned by calling SetStatusL 
    * function of MSmlDmCallback callback interface. This makes it 
    * possible to buffer the commands.
    * However, all the status codes for buffered commands must be returned
    * at the latest when the CompleteOutstandingCmdsL() of adapter is 
    * called.
    *
    * @since S60 3.2
    * @param aURI     URI of the command
    * @param aLUID      LUID of the object (if the adapter have 
    *                       earlier returned LUID to the DM Module).   
    * @param aStream    Argument for the command. Adapter should 
    *                       create write stream and return, when data is 
    *                       written to stream by DM agent,StreamCommittedL
    *                       is called by DM engine
    * @param aType      MIME type of the object 
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void ExecuteCommandL( const TDesC8& aURI,
                          const TDesC8& aLUID,
                          RWriteStream*& aStream,
                          const TDesC8& aType,
                          TInt aStatusRef );
                          
    /**
    * From MSmlDmAdapter.
    * The function implements copy command. The information about the 
    * success of the command should be returned by calling SetStatusL 
    * function of MSmlDmCallback callback interface. This makes it 
    * possible to buffer the commands.
    * However, all the status codes for buffered commands must be returned
    * at the latest when the CompleteOutstandingCmdsL() of adapter is 
    * called.
    *
    * @since S60 3.2
    * @param aTargetURI   Target URI for the command
    * @param aSourceLUID  LUID of the target object (if one exists, and 
    *                       if the adapter has  earlier returned a LUID to
    *                       the DM Module).   
    * @param aSourceURI   Source URI for the command
    * @param aSourceLUID  LUID of the source object (if the adapter has
    *             earlier returned a LUID to the DM Module).   
    * @param aType      MIME type of the objects
    * @param aStatusRef   Reference to correct command, i.e. this 
    *                       reference must be used when calling the 
    *                       SetStatusL of this command.
    */
    void CopyCommandL( const TDesC8& aTargetURI,
                       const TDesC8& aTargetLUID,
                       const TDesC8& aSourceURI,
                       const TDesC8& aSourceLUID,
                       const TDesC8& aType,
                       TInt aStatusRef );
    
    /**
    * From MSmlDmAdapter.
    * The function indicates start of Atomic command.
    *
    * @since S60 3.2
    */
    void StartAtomicL();

    /**
    * From MSmlDmAdapter.
    * The function indicates successful end of Atomic command. The adapter
    * should commit all changes issued between StartAtomicL and
    * CommitAtomicL
    *
    * @since S60 3.2
    */
    void CommitAtomicL();
    
    /**
    * From MSmlDmAdapter.
    * The function indicates unsuccessful end of Atomic command. The 
    * adapter should rollback all changes issued between StartAtomicL() 
    * and RollbackAtomicL(). If rollback fails for a command, adapter 
    * should use SetStatusL() to indicate it.
    *
    * @since S60 3.2
    */
    void RollbackAtomicL();
    
    /**
    * From MSmlDmAdapter.
    * Returns ETrue if adapter supports streaming otherwise EFalse.
    *
    * @since S60 3.2
    * @param aItemSize size limit for stream usage
    * @return TBool ETrue for streaming support
    */
    TBool StreamingSupport( TInt& aItemSize );

    /**
    * From MSmlDmAdapter.
    * Called when stream returned from UpdateLeafObjectL or 
    * ExecuteCommandL has been written to and committed. Not called when
    * fetching item.
    *
    * @since S60 3.2
    */
    void StreamCommittedL();
    
    /**
    * From MSmlDmAdapter.
    * The function tells the adapter that all the commands of the message
    * that can be passed to the adapter have now been passed.  This 
    * indciates that the adapter must supply status codes and results to 
    * any buffered commands. This must be done at latest by the time this 
    * function returns. This function is used at the end of SyncML 
    * messages, and during processing of Atomic.   In the case of Atomic 
    * processing, the function will be followed by a call to CommitAtomicL
    * or RollbackAtomicL.
    *
    * @since S60 3.2
    */
    void CompleteOutstandingCmdsL();

 	/**
    * Implementation for cleanup item.
    * Resets and destroys array of the RCSE
    * entries. 
    * @param anArray RPointerArray pointer.
    */
    static void ResetAndDestroyEntries( TAny* anArray );

protected:

private:

    /**
    * Constructor.
    *
    * @param aEcomArguments 
    */
    CNSmlDmVCCAdapter( TAny* aEcomArguments );
  
    /**
     * Second-phase constructor.
     */
    void ConstructL();
  
    /**
     * Gets last uri segment.
     *
     * @since S60 3.2
     * @param aURI Descriptor to handle
     * @return integer
     */
     const TPtrC8 LastURISeg( const TDesC8& aURI );
     
     /**
      * Gets last uri segment.
      *
      * @since S60 3.2
      * @param aURI Descriptor to handle
      * @param aResult Object data.
      * @return Error status enumeration
      */
     CSmlDmAdapter::TError FetchObjectL( const TDesC8& aURI, 
                       CBufBase& aResult );
     
     /**
      * Converts Utf8 to Unicode.
      *
      * @since S60 3.2
      * @param aSource Descriptor to handle containing UTF8 data
      * @return Handle to Unicode data
      */
     HBufC* ConvertToUnicodeL( const TDesC8& aSource );
     
     /**
      * Fetches Voip profile reference
      * @param aUri Uri of the wanted object
      * @param aObject The result is inserted to this buffer
      * @return Error code
      */
     CSmlDmAdapter::TError FetchVoipConRefL( const TDesC8& aUri, 
                                             CBufBase& aObject );
     
     /**
      * Updates Voip profile reference (ServiceId)
      * @param aUri Uri of the wanted object
      * @param aObject The result is inserted to this buffer
      * @return Error code
      */
     CSmlDmAdapter::TError UpdateVoipConRefL( const TDesC8& aUri, 
                                              const TDesC8& aObject);
     
     /**
      * Removes separator from the end of URI.
      * @param aURI Reference to the URI.
      */
     static void RemoveLastSeparator( TPtr8& aURI );

private: // data

    VCC_UNITTEST( T_CNSmlDmVCCAdapter )

    /**
     * VCC SP Settings object 
     * own
     * @see VccUtils
     */
    CVccSPSettings* iVCCSettings;
    
    /**
     * Temporary object for converting Utf8 string to Unicode 
     * own
     */
    HBufC* iTempBuf;
    };

#endif // C_NSMLDMVCCADAPTER_H
