#include <Controller.h>

Controller::Controller(sf::RenderWindow& w): m_window(w) {
    iniSettings();

    //Log
    m_log_closed = false;
}

Controller::~Controller() {
    pushLogMsg("Controller destroyed");
}

void Controller::iniSettings() {
}

void Controller::toggleLogWindow() {
    m_log_closed = !m_log_closed;
};

bool Controller::isLogClosed(){
    return m_log_closed;
};


void Controller::setActiveWindow(ActiveWindow i) {
    if(i>ActiveWindow::AW_LAST) {
        m_activeWindow = ActiveWindow::AW_FIRST;
    } else {
        m_activeWindow = i;
    }
}
ActiveWindow Controller::getActiveWindow() {
    return m_activeWindow;
}

void Controller::setActiveGameWindow(ActiveGameWindow i) {
    if(i>ActiveGameWindow::AGW_LAST) {
    	m_activeGameWindow = ActiveGameWindow::AGW_FIRST;
    } else {
    	m_activeGameWindow = i;
    }
}
ActiveGameWindow Controller::getActiveGameWindow() {
    return m_activeGameWindow;
}

sf::Vector2f Controller::getScale() {
    return m_scale;
}

void Controller::setScale(sf::Vector2f scale) {
    m_scale = scale;
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
void Controller::pushLogMsg(float f, std::string type) {
    std::string msg = std::to_string(f);
	pushLogMsg(msg, type);
};
void Controller::replaceLastLogMsg(std::string msg, std::string type) {
    if(m_log_msgList.size() > 0 && m_log_msgTypeList.size() > 0) {
        //m_log_msgList.insert(m_log_msgList.size() - 1,msg);
        //m_log_msgTypeList.insert(m_log_msgTypeList.size() - 1,msg);
    }
};

void Controller::setView(sf::View view) {
    getWindow().setView(view);
}

void Controller::setDefaultView() {
    getWindow().setView(getWindow().getDefaultView());
}

