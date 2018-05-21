#ifndef OPTIONSMENUBUTTON_H
#define OPTIONSMENUBUTTON_H

#include <QToolButton>

QT_BEGIN_HEADER
class UrlPixmap;
QT_END_NAMESPACE

class OptionsMenuButton : public QToolButton
{
    Q_OBJECT
public:
    explicit OptionsMenuButton(QWidget *parent = nullptr);
    ~OptionsMenuButton();

    QMenu* getMenu() { return m_pOptionsMenu; }

signals:

public slots:

private:
    UrlPixmap *m_pCogPixmap;
    QMenu *m_pOptionsMenu;
};

#endif // OPTIONSMENUBUTTON_H
