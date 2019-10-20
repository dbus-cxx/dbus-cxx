#!/bin/bash
#
# This script will attempt to auto-upgrade your source code to dbus-cxx version 2.0
# It does not fix everything, but it will fix most common usages
#
# usage: auto-upgrade filename

FILE=$1
sed -i 's/DBus::Dispatcher::pointer/std::shared_ptr<DBus::Dispatcher>/g' $FILE
sed -i 's/DBus::Connection::pointer/std::shared_ptr<DBus::Connection>/g' $FILE
sed -i 's/DBus::ObjectProxy::pointer/std::shared_ptr<DBus::ObjectProxy>/g' $FILE
sed -i 's/DBus::Object::pointer/std::shared_ptr<DBus::Object>/g' $FILE
sed -i 's/DBus::FileDescriptor::pointer/std::shared_ptr<DBus::FileDescriptor>/g' $FILE
sed -i 's/DBus::MethodProxy<\(.*\)>::pointer/std::shared_ptr<DBus::MethodProxy<\1>>/g' $FILE
sed -i 's/DBus::signal_proxy<\(.*\)>::pointer/std::shared_ptr<DBus::signal_proxy<\1>>/g' $FILE
sed -i 's/DBus::signal<\(.*\)>::pointer/std::shared_ptr<DBus::signal<\1>>/g' $FILE
sed -i 's/DBus::CallMessage::pointer/std::shared_ptr<DBus::CallMessage>/g' $FILE
sed -i 's/DBus::ReturnMessage::pointer/std::shared_ptr<DBus::ReturnMessage>/g' $FILE
sed -i 's/DBus::Message::pointer/std::shared_ptr<DBus::Message>/g' $FILE
sed -i 's/DBus::Message::const_pointer/std::shared_ptr<const DBus::Message>/g' $FILE
sed -i 's/DBus::Message::iterator/DBus::MessageIterator/g' $FILE
sed -i 's/DBus::PendingCall::pointer/std::shared_ptr<DBus::PendingCall>/g' $FILE
sed -i 's/DBus::ErrorMessage::pointer/std::shared_ptr<DBus::ErrorMessage>/g' $FILE
sed -i 's/DBus::MethodProxyBase::pointer/std::shared_ptr<DBus::MethodProxyBase>/g' $FILE
sed -i 's/DBus::SignalMessage::pointer/std::shared_ptr<DBus::SignalMessage>/g' $FILE
sed -i 's/DBus::SignalMessage::const_pointer/std::shared_ptr<const DBus::SignalMessage>/g' $FILE
sed -i 's/DBus::signal_proxy_simple::pointer/std::shared_ptr<DBus::signal_proxy_base>/g' $FILE
sed -i 's/DBus::Path::Decomposed/std::vector<std::string>/g' $FILE
