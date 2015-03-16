#include "shadereditor.h"
#include "settings.h"

#include <QResource>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

ShaderEditor::ShaderEditor(QWidget* parent)
	: QDialog(parent),
	  m_pair(-1),
	  m_program(-1)
{
	m_ui.setupUi(this);
	m_ui.errors->hide();
	
	connect(m_ui.pair, SIGNAL(currentIndexChanged(int)), SLOT(programChanged()));
	connect(m_ui.program, SIGNAL(currentIndexChanged(int)), SLOT(programChanged()));
	connect(m_ui.saveButton, SIGNAL(clicked()), SLOT(saveClicked()));
	connect(m_ui.compileButton, SIGNAL(clicked()), SLOT(compileClicked()));
	
	loadCode(0, 0, ":/data/shaders/terrainVert.cg");
	loadCode(0, 1, ":/data/shaders/terrainFrag.cg");
	loadCode(1, 0, ":/data/shaders/waterVert.cg");
	loadCode(1, 1, ":/data/shaders/waterFrag.cg");
	loadCode(2, 0, ":/data/shaders/worldObjectVert.cg");
	loadCode(2, 1, ":/data/shaders/worldObjectFrag.cg");
	
	programChanged();
}

ShaderEditor::~ShaderEditor()
{
}

void ShaderEditor::loadCode(int pair, int program, const QString& resourceName)
{
	QResource resource(resourceName);
	
	// Null terminate the program source
	char* source = new char[resource.size() + 1];
	memcpy(source, resource.data(), resource.size());
	source[resource.size()] = 0;
	
	m_code[pair][program] = QString(source);
	
	delete[] source;
}

void ShaderEditor::programChanged()
{
	if (m_pair != -1)
		m_code[m_pair][m_program] = m_ui.code->toPlainText();
	m_pair = m_ui.pair->currentIndex();
	m_program = m_ui.program->currentIndex();
		
	m_ui.code->setText(m_code[m_pair][m_program]);
}

void ShaderEditor::saveClicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save as", QString(), "Cg source (*.cg)");
	
	if (fileName.isEmpty())
		return;
	
	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream stream(&file);
	stream << m_ui.code->toPlainText();
	file.close();
}

void ShaderEditor::compileClicked()
{
	// TODO
	/*ShaderPair* pair = Shaders::instance()->pair((Shaders::Pair) m_ui.pair->currentIndex());
	
	pair->replaceProgram((ShaderPair::Shader) m_ui.program->currentIndex(), m_ui.code->toPlainText());
	
	if (Shaders::compilerErrors.isNull())
	{
		m_ui.errors->hide();
		Shaders::instance()->setFogRange(Settings::instance()->fogStart(), Settings::instance()->farClippingDistance());
	}
	else
	{
		m_ui.errors->setText(Shaders::compilerErrors);
		m_ui.errors->show();
	}*/
}


