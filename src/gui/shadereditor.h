#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include <QDialog>

#include "ui_shadereditor.h"

class ShaderEditor : public QDialog
{
	Q_OBJECT
public:
	ShaderEditor(QWidget* parent);
	~ShaderEditor();
	
private slots:
	void compileClicked();
	void saveClicked();
	void programChanged();
	
private:
	void loadCode(int pair, int program, const QString& resourceName);
	
	Ui_ShaderEditor m_ui;
	
	QString m_code[3][2];
	int m_pair;
	int m_program;
};

#endif
