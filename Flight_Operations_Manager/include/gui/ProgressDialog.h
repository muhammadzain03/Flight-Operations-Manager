// This header defines the `ProgressDialog` class, which is a custom dialog
// used to display progress during long-running operations in the Flight Operations Manager application.
// It includes a progress bar and a message label, and can be configured for determinate or indeterminate progress.

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTimer>

class ProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget* parent = nullptr);

    // Progress bar control
    void setProgress(int value);
    void setMaximum(int max);
    void setText(const QString& text);
    
    // Error handling
    static void showError(QWidget* parent, const QString& title, const QString& message);
    static bool showConfirmation(QWidget* parent, const QString& title, const QString& message);
    static void showWarning(QWidget* parent, const QString& title, const QString& message);
    
    // Progress updates
    void startIndeterminate();
    void stopIndeterminate();
    void complete(const QString& message = QString());

private slots:
    void updateIndeterminate();

private:
    QProgressBar* progressBar;
    QLabel* messageLabel;
    QPushButton* cancelButton;
    QTimer* indeterminateTimer;
    int indeterminateValue;
};

#endif // PROGRESSDIALOG_H 