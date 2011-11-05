// WARNING: This file is for documentation purpose - it's a beginning of a rewrite of the messaging-module

#ifndef AG_MESSAGE_H_
#define AG_MESSAGE_H_

#include <rk_string.h>

#include <map>
#include <set>
#include <list>
#include <SDL.h>

class AGMessageQueue;
class AGMessage;

/**
 * AGMessageData is the base-class for any data you (might) want to pass
 * with a message. Example: AGMessageDataSDL
 */
class AGMessageData
{
public:
  AGMessageData(size_t pID);
  virtual ~AGMessageData() {}

  size_t getMessageID() const;
private:
  size_t mID;
};

/**
 * The data of an sdl-event - should be used only for communication between
 * sdl-event-producer and screen
 * (TODO: Widgets ???)
 * */
class AGMessageDataSDL:public AGMessageData
{
public:
  AGMessageDataSDL(size_t pID,const SDL_Event &pEvent);
private:
  SDL_Event mEvent;
};

class AGMessageDataReceipt:public AGMessageData
{
public:
  AGMessageDataReceipt(size_t pID,size_t pSentID);

  size_t getSentMessageID() const;
private:
  size_t mSentID;
};

/**
 * Because it's hard to do multiple inheritance with 2 classes with a common base-class,
 * AGMessageTransceiver is the only class as a base class for your messaging classes.
 * (You won't have any AGReceiver or AGSender)
 * Ok, here's the description: Any object that wants to pass or receive messages should
 * be derived from this one.
 */
class AGMessageTransceiver 
{
  AGMessageTransceiver(AGMessageQueue *pQueue);
  virtual ~AGMessageTransceiver() throw();
  void sendMessage(const AGString &pName,AGMessageData *pData=0);

  // generate an empty message data object
  AGMessageData *makeEmptyMessageData();
  /// generate a AGMessageDataSDL from an sdl-event
  AGMessageData *makeMessageData(const SDL_Event &event);

protected:
  virtual void messageAccepted(const AGMessage &pMessage,const std::list<AGMessageTransceiver*> &pReceivers);
  virtual bool receiveMessage(const AGMessage &pMessage);
private:
  void queueDiscard();
  size_t getNewMessageID();

  AGMessageQueue *mMessageQueue;
  friend class AGMessageQueue;
};

/**
 * AGMessageSource is the source of a message, which consists of two parts:
 * - mSender - the sender of the message
 * - mName - the name of the event, that was called
 * */
class AGMessageSource
{
  AGMessageTransceiver *mSender;
  AGString mName;
public:
  AGMessageSource():mSender(0) {}
  AGMessageSource(AGMessageTransceiver *pSender,const AGString &pName);

  bool operator<(const AGMessageSource &pSource) const;
  bool operator==(const AGMessageSource &pSource) const;

  AGString getName() const;
  AGMessageTransceiver *getSender() const;
};

/**
 * AGMessage is an actual message, which consists of a
 * AGMessagSource and a pointer to a AGMessageData object.
 * the AGMessageData-object is owned by AGMessage and shouldn't
 * be deleted or modified!
 * */
class AGMessage
{
  AGMessageSource mSource;
  AGMessageData *mData;
public:
  AGMessage(const AGMessageSource &pSource,AGMessageData *pData);
  ~AGMessage();

  const AGMessageSource &getSource() const;
  AGMessageTransceiver *getSender();
private:
  AGMessage(const AGMessage&):mData(0)
  {}
};


/**
 * A Message queue is a central element of an application. It's the central
 * provider for message-passing.
 * It manages a list of all its messaging clients and their connections.
 * Apart from this it has a queue of messages. YOu can enqueue a message with
 * send(.) and let it let a single transmission to a receiver (until queueEmpty is true)
 * */
class AGMessageQueue
{
  typedef std::set<AGMessageTransceiver*> ReceiverList;
  std::map<AGMessageSource,ReceiverList> mConnects;
  std::list<AGMessage*> mMessages;
  std::set<AGMessageTransceiver*> mTransceivers;
  size_t mMessageID;

public:
  AGMessageQueue();

  void connect(const AGMessageSource &pSource,
      AGMessageTransceiver *pReceiver);
  void distconnect(const AGMessageSource &pSource,
      AGMessageTransceiver *pReceiver);

  void send(AGMessage *pMessage);
  bool doSingleMessage();
  bool queueEmpty() const;

private:
  void insertTransceiver(AGMessageTransceiver *pTransceiver);
  void eraseTransceiver(AGMessageTransceiver *pTransceiver);

  bool receiverOk(AGMessageTransceiver *pTransceiver) const;
  size_t getNewMessageID();

  friend class AGMessageTransceiver;
};

#endif /*AG_MESSAGE_H_*/
