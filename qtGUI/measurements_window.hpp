#ifndef MEASUREMENTS_WINDOW_HPP
#define MEASUREMENTS_WINDOW_HPP

#include <QTreeWidget>

class measurements_window : public QTreeWidget
{
public:
    measurements_window();
    std::vector<unsigned int> get_selected_measurements();
};

#endif // MEASUREMENTS_WINDOW_HPP
