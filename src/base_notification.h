#ifndef BASE_NOTIFICATION_H
#define BASE_NOTIFICATION_H

//
//	Includes
//
#include <list>
#include <iostream>
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace base {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Notification definition
//
template <typename TSender, typename... TArguments>
class TNotification
{
private:
	class CReceiverPlaceholder
	{};
	typedef void (CReceiverPlaceholder::* CallbackPlaceholder)(TSender const& sender, TArguments... args);

	struct SReceiverInfo
	{
		CReceiverPlaceholder*	pReceiver;
		CallbackPlaceholder		pfnCallback;
	};

	std::list<SReceiverInfo>	m_lstReceivers;

public:
	template <typename TReceiver>
	void Add(TReceiver& oReceiver, void (TReceiver::*CallbackProc)(TSender const& oSender, TArguments... args))
	{
		SReceiverInfo tRcvInfo;
		tRcvInfo.pReceiver = reinterpret_cast<CReceiverPlaceholder*>(&oReceiver);
		tRcvInfo.pfnCallback = reinterpret_cast<CallbackPlaceholder>(CallbackProc);
		m_lstReceivers.push_back(tRcvInfo);
	}

	template <typename TReceiver>
	void Remove(TReceiver& oReceiver, void (TReceiver::*CallbackProc)(TSender const& oSender, TArguments... args))
	{
		SReceiverInfo tRcvInfo;
		tRcvInfo.pReceiver = reinterpret_cast<CReceiverPlaceholder*>(&oReceiver);
		tRcvInfo.pfnCallback = reinterpret_cast<CallbackPlaceholder>(CallbackProc);
		m_lstReceivers.remove(tRcvInfo);
	}

	void Notify(TSender const& oSender, TArguments... args) const
	{
		for (SReceiverInfo const& oRcvInfo : m_lstReceivers)
		{
			(oRcvInfo.pReceiver->*oRcvInfo.pfnCallback)(oSender, args...);
		}
	}
};


//
// Notification usage
//
class CSender1
{
public:
	TNotification<CSender1, int, int> evnSomethingChanged;

	void DoSomething()
	{
		evnSomethingChanged.Notify(*this, int(0), int(1));
	}

	void ThankMeForNotifingYou() const
	{
		std::cout << "ThankMeForNotifingYou" << std::endl;
	}
};

class CListener1
{
public:
	void onSomethingChanged(CSender1 const&, int, int);

	int m_a, m_b;
};

class CListener2
{
public:
	void onSomethingChangedA(CSender1 const&, int, int);
	void onSomethingChangedB(CSender1 const&, int, int);

	int m_a, m_b;
};

void CListener1::onSomethingChanged(CSender1 const& oSender, int a, int b)
{
	m_a = a;
	m_b = b;
	oSender.ThankMeForNotifingYou();
}

void CListener2::onSomethingChangedA(CSender1 const& oSender, int a, int b)
{
	m_a = a;
	m_b = b;
	oSender.ThankMeForNotifingYou();
}

void CListener2::onSomethingChangedB(CSender1 const& oSender, int a, int b)
{
	m_a = a;
	m_b = b;
	oSender.ThankMeForNotifingYou();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //BASE_NOTIFICATION_H
