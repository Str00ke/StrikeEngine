#pragma once


class StrikeApp
{
public:
	StrikeApp();

	bool IsRunning() const;
	static StrikeApp* Instance();
	void OnQuit();

private:
	bool m_isRunning;
	static StrikeApp* m_Instance;
};

