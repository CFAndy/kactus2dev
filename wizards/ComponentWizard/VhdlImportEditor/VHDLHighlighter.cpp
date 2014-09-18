//-----------------------------------------------------------------------------
// File: VHDLHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Source file highlighter for VHDL files.
//-----------------------------------------------------------------------------

#include "VHDLHighlighter.h"

#include <Plugins/PluginSystem/HighlightSource.h>

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::VHDLHighlighter()
//-----------------------------------------------------------------------------
VHDLHighlighter::VHDLHighlighter(QPlainTextEdit* display, QObject *parent)
    : QObject(parent),
    display_(display)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::~VHDLHighlighter()
//-----------------------------------------------------------------------------
VHDLHighlighter::~VHDLHighlighter()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::registerParser()
//-----------------------------------------------------------------------------
void VHDLHighlighter::registerHighlightSource(QObject* highlightSource)
{
    connect(highlightSource, SIGNAL(highlight(QString const&, QColor const&)), 
        this, SLOT(applyHighlight(QString const&, QColor const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::setFontColor()
//-----------------------------------------------------------------------------
void VHDLHighlighter::applyFontColor(QString const& text, QColor const& color)
{
    int beginIndex = display_->toPlainText().indexOf(text);
    if (beginIndex != -1)
    {
        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(beginIndex);
        cursor.setPosition(beginIndex + text.length(), QTextCursor::KeepAnchor);

        QTextCharFormat highlighFormat;
        highlighFormat.setForeground(QBrush(color));
        cursor.setCharFormat(highlighFormat);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::highlight()
//-----------------------------------------------------------------------------
void VHDLHighlighter::applyHighlight(QString const& text, QColor const& highlightColor) const
{
    int beginIndex = display_->toPlainText().indexOf(text);
    if (beginIndex != -1)
    {
        QTextCharFormat highlighFormat;
        highlighFormat.setBackground(QBrush(highlightColor));

        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(beginIndex);
        cursor.setPosition(beginIndex + text.length(), QTextCursor::KeepAnchor);
        cursor.setCharFormat(highlighFormat);
    }
}
