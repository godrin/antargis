#include "ag_message.h"
#include "rk_debug.h"

AGMessageData::AGMessageData(size_t pID):
  mID(pID)
  {
  }


AGMessageDataSDL::AGMessageDataSDL(size_t pID,const SDL_Event &pEvent):
  AGMessageData(pID),
  mEvent(pEvent)
  {

  }


/////////////////////////////////////////////////
// AGMessageTransceiver
/////////////////////////////////////////////////
AGMessageTransceiver::AGMessageTransceiver(AGMessageQueue *pQueue):
  mMessageQueue(pQueue)
  {
    assert(mMessageQueue);
    mMessageQueue->insertTransceiver(this);
  }
AGMessageTransceiver::~AGMessageTransceiver() throw()
  {
    if(mMessageQueue)
      mMessageQueue->eraseTransceiver(this);
  }

void AGMessageTransceiver::sendMessage(const AGString &pName,AGMessageData *pData)
  {
    assert(mMessageQueue);

  }
bool AGMessageTransceiver::receiveMessage(const AGMessage &pMessage)
  {
    assert(mMessageQueue);
    // TODO: relay message
  }
void AGMessageTransceiver::messageAccepted(const AGMessage &pMessage,const std::list<AGMessageTransceiver*> &pReceivers)
  {

  }

void AGMessageTransceiver::queueDiscard()
  {
    assert(mMessageQueue);
    mMessageQueue=0;
  }

AGMessageData *AGMessageTransceiver::makeEmptyMessageData()
  {
    return new AGMessageData(getNewMessageID());
  }

AGMessageData *AGMessageTransceiver::makeMessageData(const SDL_Event &event)
  {
    return new AGMessageDataSDL(getNewMessageID(),event);
  }

size_t AGMessageTransceiver::getNewMessageID()
  {
    assert(mMessageQueue);
    return mMessageQueue->getNewMessageID();
  }

/////////////////////////////////////////////////
// AGMessageSource
/////////////////////////////////////////////////

AGMessageSource::AGMessageSource(AGMessageTransceiver *pSender,const AGString &pName):
  mSender(pSender),mName(pName)
  {
  }

bool AGMessageSource::operator<(const AGMessageSource &pSource) const
{
  return (mSender<pSource.mSender) || (mSender==pSource.mSender && mName<pSource.mName);
}
bool AGMessageSource::operator==(const AGMessageSource &pSource) const
{
  return mSender==pSource.mSender && mName==pSource.mName;
}

AGString AGMessageSource::getName() const
{
  return mName;
}

AGMessageTransceiver *AGMessageSource::getSender() const
{
  return mSender;
}



/////////////////////////////////////////////////
// AGMessage
/////////////////////////////////////////////////

AGMessage::AGMessage(const AGMessageSource &pSource,AGMessageData *pData):mSource(pSource),mData(pData)
{
}

AGMessage::~AGMessage()
  {
    checkedDelete(mData);
  }

const AGMessageSource &AGMessage::getSource() const
{
  return mSource;
}

AGMessageTransceiver *AGMessage::getSender()
  {
    return mSource.getSender();
  }


// class AGMessageQueue

AGMessageQueue::AGMessageQueue():
  mMessageID(0)
  {

  }


void AGMessageQueue::connect(const AGMessageSource &pSource,
    AGMessageTransceiver *pReceiver)
  {
    mConnects[pSource].insert(pReceiver);
  }
void AGMessageQueue::distconnect(const AGMessageSource &pSource,
    AGMessageTransceiver *pReceiver)
  {
    mConnects[pSource].erase(pReceiver);
  }

/**
 * pMessage must be created outside of send, but its ownage
 * is transfered to AGMessageQueue. AGMessageQueue will remove it,
 * when it's ok.
 */
void AGMessageQueue::send(AGMessage *pMessage)
  {
    mMessages.push_back(pMessage);
  }

bool AGMessageQueue::queueEmpty() const
{
  return mMessages.empty();
}


bool AGMessageQueue::doSingleMessage()
  {
    if(queueEmpty())
      return false;

    AGMessage *currentMessage=*mMessages.begin();
    assert(currentMessage);
    const ReceiverList &receiverList=mConnects[currentMessage->getSource()];

    std::list<AGMessageTransceiver*> acceptingReceivers;

    for(ReceiverList::const_iterator receiverIter=receiverList.begin();receiverIter!=receiverList.end();receiverIter++)
      {
        AGMessageTransceiver *receiver=*receiverIter;
        if(receiverOk(receiver))
          {
            bool accepted=receiver->receiveMessage(*currentMessage);
            if(accepted)
              acceptingReceivers.push_back(receiver);
          }
      }
    AGMessageTransceiver *sender=currentMessage->getSender();
    if(receiverOk(sender))
      {
        sender->messageAccepted(*currentMessage,acceptingReceivers);
      }

    return true;
  }


bool AGMessageQueue::receiverOk(AGMessageTransceiver *pTransceiver) const
{
  return mTransceivers.find(pTransceiver)!=mTransceivers.end();
}


void AGMessageQueue::insertTransceiver(AGMessageTransceiver *pTransceiver)
  {
    mTransceivers.insert(pTransceiver);
  }

void AGMessageQueue::eraseTransceiver(AGMessageTransceiver *pTransceiver)
  {
    mTransceivers.erase(pTransceiver);
  }

size_t AGMessageQueue::getNewMessageID()
  {
    return mMessageID++;
  }
