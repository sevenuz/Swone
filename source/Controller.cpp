#include <Controller.h>

Controller::Controller(sf::RenderWindow& w): m_window(w) {
    iniSettings();

    //Log
    m_log_closed = false;
}

Controller::~Controller() {
    //dtor
}

void Controller::iniSettings() {
}

void Controller::toggleLogWindow() {
    m_log_closed = !m_log_closed;
};

bool Controller::isLogClosed(){
    return m_log_closed;
};


void Controller::setActiveWindow(unsigned int i) {
    if(i>ActiveWindow::LAST) {
        m_activeWindow = ActiveWindow::FIRST;
    } else {
        m_activeWindow = i;
    }
}
unsigned int Controller::getActiveWindow() {
    return m_activeWindow;
}

void Controller::setActiveGameWindow(unsigned int i) {
    if(i>ActiveGameWindow::LAST) {
    	m_activeGameWindow = ActiveGameWindow::FIRST;
    } else {
    	m_activeGameWindow = i;
    }
}
unsigned int Controller::getActiveGameWindow() {
    return m_activeGameWindow;
}


sf::RenderWindow& Controller::getWindow() {
    return m_window;
}



//Log
std::vector<std::string>* Controller::getMsgList() {
    return &m_log_msgList;
};
std::vector<std::string>* Controller::getMsgTypeList() {
    return &m_log_msgTypeList;
};
std::string& Controller::getLastLogMsg() {
    return m_log_msgList.back();
};
void Controller::clearLog() {
    m_log_msgList.clear();
    m_log_msgTypeList.clear();
};
void Controller::pushLogMsg(std::string msg, std::string type) {
	std::cout << msg << std::endl;
    m_log_msgList.push_back(msg);
    m_log_msgTypeList.push_back(type);
};
void Controller::replaceLastLogMsg(std::string msg, std::string type) {
    if(m_log_msgList.size() > 0 && m_log_msgTypeList.size() > 0) {
        //m_log_msgList.insert(m_log_msgList.size() - 1,msg);
        //m_log_msgTypeList.insert(m_log_msgTypeList.size() - 1,msg);
    }
};
