#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

typedef void(*JMS_Notify_func)(LPVOID lparam,char *cMessage);

class JMSProducer : 
	public MessageListener,public Runnable {
private:
	/*CountDownLatch latch;
	CountDownLatch doneLatch;*/
	Connection* connection;
	Session* session;
	Destination* destination;
	Destination* reply;
	MessageProducer* producer;
	MessageConsumer* consumer;
	int numMessages;
	bool useTopic;
	bool sessionTransacted;
	std::string brokerURI;

	LPVOID m_lpUserData;
	JMS_Notify_func m_pJMSNotifyfunc;

private:

	JMSProducer(const JMSProducer&);
	JMSProducer& operator=(const JMSProducer&);

public:

	JMSProducer(const std::string& brokerURI, int numMessages, bool useTopic = false, bool sessionTransacted = false) :
	  connection(NULL),
		  session(NULL),
		  destination(NULL),
		  producer(NULL),
		  numMessages(numMessages),
		  useTopic(useTopic),
		  sessionTransacted(sessionTransacted),
		  brokerURI(brokerURI) {
	  }

	  virtual ~JMSProducer(){
		  cleanup();
	  }

	  void close() {
		  this->cleanup();
	  }

	  void Sendms(string strMessage);
	  //{
		 // try
		 // {
			//  std::auto_ptr<TextMessage> message(session->createTextMessage(strMessage));
			//  message->setCMSReplyTo(reply);
			//  producer->send(message.get());
		 // }
		 // catch (CMSException& e) {
			//  //latch.countDown();
			//  e.printStackTrace();
		 // }
		 // 
	  //}

	  void setCallBackParam(LPVOID lpparam,JMS_Notify_func pJMSNotifyfunc)
	  {
		  m_lpUserData = lpparam;
		  m_pJMSNotifyfunc = pJMSNotifyfunc;
	  }

	  virtual void run() ;

	  void onMessage(const Message* message) ;


private:

	void cleanup() {

		if (connection != NULL) {
			try {
				connection->close();
			} catch (cms::CMSException& ex) {
				ex.printStackTrace();
			}
		}

		// Destroy resources.
		try {
			delete destination;
			destination = NULL;
			delete producer;
			producer = NULL;

			delete consumer;
			consumer = NULL;

			delete session;
			session = NULL;
			delete connection;
			connection = NULL;
		} catch (CMSException& e) {
			e.printStackTrace();
		}
	}
};

class JMSConsumer : public ExceptionListener,
	public MessageListener,
	public Runnable {

private:

	CountDownLatch latch;
	CountDownLatch doneLatch;
	Connection* connection;
	Session* session;
	Destination* destination;
	MessageConsumer* consumer;
	long waitMillis;
	bool useTopic;
	bool sessionTransacted;
	std::string brokerURI;

	LPVOID m_lpUserData;
	JMS_Notify_func m_pJMSNotifyfunc;

private:

	JMSConsumer(const JMSConsumer&);
	JMSConsumer& operator=(const JMSConsumer&);

public:

	JMSConsumer(const std::string& brokerURI, int numMessages, bool useTopic = false, bool sessionTransacted = false, int waitMillis = 30000) :
	  latch(1),
		  doneLatch(numMessages),
		  connection(NULL),
		  session(NULL),
		  destination(NULL),
		  consumer(NULL),
		  waitMillis(waitMillis),
		  useTopic(useTopic),
		  sessionTransacted(sessionTransacted),
		  brokerURI(brokerURI) 
	  {

	  }

	  virtual ~JMSConsumer() {
		  cleanup();
	  }

	  void setCallBackParam(LPVOID lpparam,JMS_Notify_func pJMSNotifyfunc)
	  {
		 m_lpUserData = lpparam;
		 m_pJMSNotifyfunc = pJMSNotifyfunc;
	  }

	  void close() {
		  this->cleanup();
	  }

	  void waitUntilReady() {
		  latch.await();
	  }

	  virtual void run() {

		  try {

			  // Create a ConnectionFactory
			  auto_ptr<ConnectionFactory> connectionFactory(
				  ConnectionFactory::createCMSConnectionFactory(brokerURI));

			  // Create a Connection
			  connection = connectionFactory->createConnection("admin", "jms123456");
			  connection->start();
			  connection->setExceptionListener(this);

			  // Create a Session
			  if (this->sessionTransacted == true) {
				  session = connection->createSession(Session::SESSION_TRANSACTED);
			  } else {
				  session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
			  }

			  // Create the destination (Topic or Queue)
			  if (useTopic) {
				  destination = session->createTopic("t.advanced.notify");
			  } else {
				  destination = session->createQueue("q.advanced.notify");
			  }

			  // Create a MessageConsumer from the Session to the Topic or Queue
			  consumer = session->createConsumer(destination);

			  consumer->setMessageListener(this);


			  std::cout.flush();
			  std::cerr.flush();

			  // Indicate we are ready for messages.
			  latch.countDown();

			  // Wait while asynchronous messages come in.
			  doneLatch.await(waitMillis);

		  } catch (CMSException& e) {
			  // Indicate we are ready for messages.
			  latch.countDown();
			  e.printStackTrace();
		  }
	  }

	  // Called from the consumer since this class is a registered MessageListener.
	  virtual void onMessage(const Message* message) {

		  static int count = 0;

		  try {
			  count++;
			  const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
			  string text = "";

			  if (textMessage != NULL) {
				  text = textMessage->getText();
			  } else {
				  //text = "NOT A TEXTMESSAGE!";
			  }
			  m_pJMSNotifyfunc(m_lpUserData,(char *)text.c_str());	
			  //printf("Message #%d Received: %s\n", count, text.c_str());

		  } catch (CMSException& e) {
			  e.printStackTrace();
		  }

		  // Commit all messages.
		  if (this->sessionTransacted) {
			  session->commit();
		  }

		  // No matter what, tag the count down latch until done.
		  doneLatch.countDown();
	  }



	  // If something bad happens you see it here as this class is also been
	  // registered as an ExceptionListener with the connection.
	  virtual void onException(const CMSException& ex AMQCPP_UNUSED) {
		  printf("CMS Exception occurred.  Shutting down client.\n");
		  ex.printStackTrace();
		  exit(1);
	  }

private:

	void cleanup() {
		if (connection != NULL) {
			try {
				connection->close();
			} catch (cms::CMSException& ex) {
				ex.printStackTrace();
			}
		}

		// Destroy resources.
		try {
			delete destination;
			destination = NULL;
			delete consumer;
			consumer = NULL;
			delete session;
			session = NULL;
			delete connection;
			connection = NULL;
		} catch (CMSException& e) {
			e.printStackTrace();
		}
	}
};