#pragma once

namespace JR {

struct Token {};

using MessageToken = std::shared_ptr<Token>;

class MessageEmitter {
	struct MessageEntry {
		void* callback;
		std::weak_ptr<Token> token;
	};

public:
	template <typename MessageType>
	std::shared_ptr<Token> Subscribe(std::function<void(const MessageType&)> callback) {
		uint32_t messageId = TypeId::Get<MessageType>();

		auto fn = new std::function<void(const MessageType&)>{[=](const MessageType& message) { callback(message); }};

		std::shared_ptr<Token> token = std::make_shared<Token>();

		MessageEntry entry{.callback = reinterpret_cast<void*>(fn), .token = token};

		Subscribe(messageId, entry);

		return token;
	}

	template <typename MessageType>
	void Emit(const MessageType& message) {
		uint32_t messageId = TypeId::Get<MessageType>();

		auto it = mListeners.find(messageId);
		if (it == mListeners.end()) {
			return;
		}

		for (int i = 0; i < it->second.size(); ++i) {
			auto& entry = it->second[i];

			auto cb = reinterpret_cast<std::function<void(const MessageType&)>*>(entry.callback);

			if (entry.token.expired()) {
				delete cb;
				it->second.erase(it->second.begin() + i);
				--i;
				continue;
			}

			(*cb)(message);
		}
	}

private:
	void Subscribe(uint32_t messageId, MessageEntry entry) {
		if (mListeners.find(messageId) == mListeners.end()) {
			mListeners[messageId] = {};
		}

		mListeners[messageId].push_back(entry);
	}

	std::map<uint32_t, std::vector<MessageEntry>> mListeners;
};

}  // namespace JR
