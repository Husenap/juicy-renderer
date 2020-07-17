#pragma once

namespace JR {

using WatchCallback = std::function<void()>;

class FileWatcher : public Module {
	struct FileEntry {
		WatchCallback callback;
		std::filesystem::file_time_type lastWriteTime;
	};

public:
	FileWatcher() {
		mIsRunning  = true;
		mHasChanges = false;
		mThread     = std::thread([=]() { WatchFiles(); });
	}
	~FileWatcher() {
		mIsRunning = false;
		if (mThread.joinable()) {
			mThread.join();
		}
	}

	void Watch(const std::string& filePath, WatchCallback callback) {
		std::filesystem::file_time_type lastWriteTime;
		if (std::filesystem::exists(filePath)) {
			lastWriteTime = std::filesystem::last_write_time(filePath);
		}

		mEntries[filePath] = FileEntry{.callback = callback, .lastWriteTime = lastWriteTime};
	}

	void FlushChanges() {
		if (!mHasChanges) {
			return;
		}

		for (auto& filepath : mChanges) {
			LOG_INFO("Reloading File: {}", filepath);
			mEntries[filepath].callback();
		}

		mChanges.clear();

		mHasChanges = false;
	}

private:
	void WatchFiles() {
		while (mIsRunning) {
			if (!mHasChanges) {
				for (auto& entry : mEntries) {
					if (!std::filesystem::exists(entry.first)) {
						continue;
					}

					auto currentWriteTime = std::filesystem::last_write_time(entry.first);

					if ((currentWriteTime - entry.second.lastWriteTime).count() == 0) {
						continue;
					}

					entry.second.lastWriteTime = currentWriteTime;
					mChanges.push_back(entry.first);
				}
			}

			if (mChanges.size() > 0) {
				mHasChanges = true;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	std::map<std::string, FileEntry> mEntries;
	std::vector<std::string> mChanges;
	std::thread mThread;
	volatile bool mIsRunning;
	volatile bool mHasChanges;
};

}  // namespace JR