#include "JMSNotify.h"

int activemq_init()
{
	
	return 0 ;
}

void JMSProducer::run()
{

	try {

		// Create a ConnectionFactory
		auto_ptr<ConnectionFactory> connectionFactory(
			ConnectionFactory::createCMSConnectionFactory(brokerURI));

		// Create a Connection
		connection = connectionFactory->createConnection("admin", "jms123456");
		connection->start();

		// Create a Session
		if (this->sessionTransacted) {
			session = connection->createSession(Session::SESSION_TRANSACTED);
		} else {
			session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
		}

		// Create the destination (Topic or Queue)
		if (useTopic) {
			destination = session->createTopic("t.advanced.notify");
		} else {
			string threadIdStr = Long::toString(Thread::currentThread()->getId());
			destination = session->createQueue("q.advanced.notify");
			threadIdStr+="gbgw.reply";
			reply = session->createQueue("gbgw.reply");
		}

		// Create a MessageProducer from the Session to the Topic or Queue
		if(!useTopic)
		{
			consumer = session->createConsumer(reply);
			consumer->setMessageListener(this);

			std::cout.flush();
			std::cerr.flush();

			// Indicate we are ready for messages.
			//latch.countDown();

			// Wait while asynchronous messages come in.
			//doneLatch.await(10);
		}
		producer = session->createProducer(destination);
		producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

		// Create the Thread Id String
		//string threadIdStr = Long::toString(Thread::currentThread()->getId());

		// Create a messages
		// string text = (string) "Hello world! from thread " + threadIdStr;

		//for (int ix = 0; ix < numMessages; ++ix) {
		//    std::auto_ptr<TextMessage> message(session->createTextMessage(text));
		//    message->setIntProperty("Integer", ix);
		//    //printf("Sent message #%d from thread %s\n", ix + 1, threadIdStr.c_str());
		//    producer->send(message.get());
		//}

	} catch (CMSException& e) {
		//latch.countDown();
		e.printStackTrace();
	}
}

void JMSProducer::Sendms(string strMessage)
{
	try
	{
		std::auto_ptr<TextMessage> message(session->createTextMessage(strMessage));
		message->setCMSReplyTo(reply);
		producer->send(message.get());
	}
	catch (CMSException& e) {
		//latch.countDown();
		e.printStackTrace();
	}

}

void JMSProducer::onMessage(const Message* message)
{

	static int count = 0;

	try {
		count++;
		const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
		string text = "";

		if (textMessage != NULL) {
			text = textMessage->getText();
		} else {
			text = "NOT A TEXTMESSAGE!";
		}
		m_pJMSNotifyfunc(m_lpUserData,(char *)text.c_str());		
		printf("Message #%d Received: %s\n", count, text.c_str());

	} catch (CMSException& e) {
		e.printStackTrace();
	}

	// Commit all messages.
	if (this->sessionTransacted) {
		session->commit();
	}

	// No matter what, tag the count down latch until done.
	//doneLatch.countDown();
}