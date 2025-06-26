// This file contains the implementation for the `ProgressDialog` class,
// which provides a customizable dialog to display progress indicators for long-running operations.
// It supports both determinate and indeterminate progress modes, enhancing user experience
// during background tasks in the Flight Operations Manager application.

#include "gui/ProgressDialog.h"
#include <QtWidgets/QMessageBox>
#include <QShortcut>

ProgressDialog::ProgressDialog(QWidget* parent)
    : QDialog(parent)
    , indeterminateValue(0)
{
    setWindowTitle(tr("Operation in Progress"));
    setModal(true);
    
    auto layout = new QVBoxLayout(this);
    
    messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);
    layout->addWidget(messageLabel);
    
    progressBar = new QProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    layout->addWidget(progressBar);
    
    cancelButton = new QPushButton(tr("Cancel"), this);
    layout->addWidget(cancelButton);
    
    indeterminateTimer = new QTimer(this);
    connect(indeterminateTimer, &QTimer::timeout, this, &ProgressDialog::updateIndeterminate);
    
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    setLayout(layout);
    setStyleSheet(R"(
        QDialog {
            background-color: #2c3e50;
            color: #ecf0f1;
            font-family: 'Segoe UI';
        }
        QLabel {
            color: #ecf0f1;
            font-size: 13px;
            font-weight: 500;
        }
        QProgressBar {
            background-color: #34495e;
            color: #ecf0f1;
            border: 1px solid #2c3e50;
            border-radius: 4px;
            font-size: 13px;
        }
        QPushButton {
            background-color: #f39c12;
            color: #2c3e50;
            border: none;
            border-radius: 4px;
            padding: 10px 24px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #f1c40f;
        }
        QPushButton:pressed {
            background-color: #d35400;
        }
    )");
    // Tooltips for accessibility
    messageLabel->setToolTip(tr("Displays the current operation message."));
    progressBar->setToolTip(tr("Shows the progress of the current operation."));
    cancelButton->setToolTip(tr("Cancel the operation and close this dialog (Esc)"));
    // Keyboard shortcut for Esc
    (void)new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(reject()));
}

void ProgressDialog::setProgress(int value) {
    progressBar->setValue(value);
}

void ProgressDialog::setMaximum(int max) {
    progressBar->setMaximum(max);
}

void ProgressDialog::setText(const QString& text) {
    messageLabel->setText(text);
}

void ProgressDialog::showError(QWidget* parent, const QString& title, const QString& message) {
    QMessageBox::critical(parent, title, message);
}

bool ProgressDialog::showConfirmation(QWidget* parent, const QString& title, const QString& message) {
    return QMessageBox::question(parent, title, message,
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

void ProgressDialog::showWarning(QWidget* parent, const QString& title, const QString& message) {
    QMessageBox::warning(parent, title, message);
}

void ProgressDialog::startIndeterminate() {
    progressBar->setMaximum(0);
    indeterminateTimer->start(50);
}

void ProgressDialog::stopIndeterminate() {
    indeterminateTimer->stop();
    progressBar->setMaximum(100);
}

void ProgressDialog::complete(const QString& message) {
    stopIndeterminate();
    progressBar->setValue(100);
    if (!message.isEmpty()) {
        setText(message);
    }
    cancelButton->setText(tr("Close"));
}

void ProgressDialog::updateIndeterminate() {
    indeterminateValue = (indeterminateValue + 1) % 100;
    progressBar->setValue(indeterminateValue);
} 