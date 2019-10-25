#!/bin/bash
#
# This script will attempt to auto-upgrade your source code to dbus-cxx version 2.0
# It does not fix everything, but it will fix most common usages
#
# usage: auto-upgrade filename

FILE=$1

# Type changes
sed -i 's/DBus::Dispatcher::pointer/std::shared_ptr<DBus::Dispatcher>/g' $FILE
sed -i 's/DBus::Connection::pointer/std::shared_ptr<DBus::Connection>/g' $FILE
sed -i 's/DBus::ObjectProxy::pointer/std::shared_ptr<DBus::ObjectProxy>/g' $FILE
sed -i 's/DBus::Object::pointer/std::shared_ptr<DBus::Object>/g' $FILE
sed -i 's/DBus::FileDescriptor::pointer/std::shared_ptr<DBus::FileDescriptor>/g' $FILE
sed -i 's/DBus::MethodProxy<\(.*\)>::pointer/std::shared_ptr<DBus::MethodProxy<\1>>/g' $FILE
sed -i 's/DBus::Method<\(.*\)>::pointer/std::shared_ptr<DBus::Method<\1>>/g' $FILE
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
sed -i 's/Error::pointer/Error/g' $FILE

# Enum changes
sed -i 's/BUS_NONE/BusType::NONE/g' $FILE
sed -i 's/BUS_SESSION/BusType::SESSION/g' $FILE
sed -i 's/BUS_SYSTEM/BusType::SYSTEM/g' $FILE
sed -i 's/BUS_STARTER/BusType::STARTER/g' $FILE
sed -i 's/CREATE_ALIAS/CreateMethod::ALIAS/g' $FILE
sed -i 's/CREATE_COPY/CreateMethod::COPY/g' $FILE
sed -i 's/TYPE_INVALID/Type::INVALID/g' $FILE
sed -i 's/TYPE_BYTE/Type::BYTE/g' $FILE
sed -i 's/TYPE_BOOLEAN/Type::BOOLEAN/g' $FILE
sed -i 's/TYPE_INT16/Type::INT16/g' $FILE
sed -i 's/TYPE_UINT16/Type::UINT16/g' $FILE
sed -i 's/TYPE_INT32/Type::INT32/g' $FILE
sed -i 's/TYPE_UINT32/Type::UINT32/g' $FILE
sed -i 's/TYPE_INT64/Type::INT64/g' $FILE
sed -i 's/TYPE_UINT64/Type::UINT64/g' $FILE
sed -i 's/TYPE_DOUBLE/Type::DOUBLE/g' $FILE
sed -i 's/TYPE_STRING/Type::STRING/g' $FILE
sed -i 's/TYPE_OBJECT_PATH/Type::OBJECT_PATH/g' $FILE
sed -i 's/TYPE_SIGNATURE/Type::SIGNATURE/g' $FILE
sed -i 's/TYPE_ARRAY/Type::ARRAY/g' $FILE
sed -i 's/TYPE_VARIANT/Type::VARIANT/g' $FILE
sed -i 's/TYPE_STRUCT/Type::STRUCT/g' $FILE
sed -i 's/TYPE_DICT_ENTRY/Type::DICT_ENTRY/g' $FILE
sed -i 's/TYPE_UNIX_FD/Type::UNIX_FD/g' $FILE
sed -i 's/CONTAINER_ARRAY/ContainerType::ARRAY/g' $FILE
sed -i 's/CONTAINER_VARIANT/ContainerType::VARIANT/g' $FILE
sed -i 's/CONTAINER_STRUCT/ContainerType::STRUCT/g' $FILE
sed -i 's/CONTAINER_DICT_ENTRY/ContainerType::DICT_ENTRY/g' $FILE
sed -i 's/INVALID_MESSAGE/MessageType::INVALID/g' $FILE
sed -i 's/CALL_MESSAGE/MessageType::CALL/g' $FILE
sed -i 's/RETURN_MESSAGE/MessageType::RETURN/g' $FILE
sed -i 's/ERROR_MESSAGE/MessageType::ERROR/g' $FILE
sed -i 's/SIGNAL_MESSAGE/MessageType::SIGNAL/g' $FILE
sed -i 's/DISPATCH_DATA_REMAINS/DispatchStatus::DATA_REMAINS/g' $FILE
sed -i 's/DISPATCH_COMPLETE/DispatchStatus::COMPLETE/g' $FILE
sed -i 's/DISPATCH_NEED_MEMORY/DispatchStatus::NEED_MEMORY/g' $FILE
sed -i 's/NOT_HANDLED/HandlerResult::NOT_HANDLED/g' $FILE
sed -i 's/HANDLER_NEEDS_MEMORY/HandlerResult::NEEDS_MEMORY/g' $FILE
sed -i 's/ HANDLED/ HandlerResult::HANDLED/g' $FILE
sed -i 's/DONT_FILTER/FilterResult::DONT_FILTER/g' $FILE
sed -i 's/FILTER_NEEDS_MEMORY/FilterResult::NEEDS_MEMORY/g' $FILE
sed -i 's/ FILTER/ FilterResult::FILTER/g' $FILE
sed -i 's/START_REPLY_FAILED/StartReply::FAILED/g' $FILE
sed -i 's/START_REPLY_SUCCESS/StartReply::SUCCESS/g' $FILE
sed -i 's/START_REPLY_ALREADY_RUNNING/StartReply::ALREADY_RUNNING/g' $FILE
sed -i 's/WATCH_READABLE/WatchStatus::READABLE/g' $FILE
sed -i 's/WATCH_WRITABLE/WatchStatus::WRITABLE/g' $FILE
sed -i 's/WATCH_ERROR/WatchStatus::ERROR/g' $FILE
sed -i 's/WATCH_HANGUP/WatchStatus::HANGUP/g' $FILE
sed -i 's/PRIMARY/PrimaryFallback::PRIMARY/g' $FILE
sed -i 's/FALLBACK/PrimaryFallback::FALLBACK/g' $FILE
sed -i 's/DBUS_REQUEST_NAME_REPLY_PrimaryFallback::PRIMARY_OWNER/DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER/g' $FILE

