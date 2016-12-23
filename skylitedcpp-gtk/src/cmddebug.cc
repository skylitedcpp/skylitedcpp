/*
 * Copyright © 2010 Mank <mank@besthub.eu>
 * Copyright © 2010 Eugene Petrov <dhamp@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * In addition, as a special exception, compiling, linking, and/or
 * using OpenSSL with this program is allowed.
 */

#include "cmddebug.hh"
#include "wulformanager.hh"
#include "WulforUtil.hh"
#include <dcpp/DebugManager.h>

using namespace std;
using namespace dcpp;

cmddebug::cmddebug():
BookEntry(Entry::CMD,_("CMD"),"cmddebug.ui")
{
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (getWidget("cmdtextview")));
    gtk_text_buffer_get_end_iter(buffer, &iter);
    cmdMark = gtk_text_buffer_create_mark(buffer, NULL, &iter, FALSE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(getWidget("hub_in_button")) ,TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(getWidget("hub_out_button")) ,TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(getWidget("client_in_button")) ,TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(getWidget("client_out_button")) ,TRUE);
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(getWidget("cmdscroll")));
    g_signal_connect(adjustment, "value_changed", G_CALLBACK(onScroll_gui), (gpointer)this);
    g_signal_connect(adjustment, "changed", G_CALLBACK(onResize_gui), (gpointer)this);

}

cmddebug::~cmddebug()
{
    DebugManager::getInstance()->removeListener(this);
}

void cmddebug::add_gui(string file)
{
    if (file.empty())
        return;

    gtk_text_buffer_get_end_iter(buffer, &iter);

    string line = Text::toUtf8("["+Util::getTimeString()+"]"+file+"\n\0");

    gtk_text_buffer_insert(buffer, &iter, line.c_str(), line.size());
    gtk_text_buffer_get_end_iter(buffer, &iter);

    // Limit size of chat text
    if (gtk_text_buffer_get_line_count(buffer) > maxLines + 1)
    {
            GtkTextIter next;
            gtk_text_buffer_get_start_iter(buffer, &iter);
            gtk_text_buffer_get_iter_at_line(buffer, &next, 1);
            gtk_text_buffer_delete(buffer, &iter, &next);
    }
    gtk_text_view_place_cursor_onscreen(GTK_TEXT_VIEW(getWidget("cmdtextview")));


}

void cmddebug::init()
{
    start();
    DebugManager::getInstance()->addListener(this);
}

void cmddebug::show()
{
    init();
}

void cmddebug::on(dcpp::DebugManagerListener::DebugCommand, const std::string& mess, int typedir, const std::string& ip) noexcept
{
        switch(typedir) {
            case dcpp::DebugManager::HUB_IN :
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("hub_in_button"))) == TRUE)
                {
                    addCmd("Hub:\t[Incoming][" + ip + "]\t\t"+mess, ip);
                }
                break;
            case dcpp::DebugManager::HUB_OUT :
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("hub_out_button"))) == TRUE)
                {
                    addCmd("Hub:\t[Outgoing][" + ip + "]\t\t"+mess, ip);
                }
                break;
            case dcpp::DebugManager::CLIENT_IN:
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("client_in_button"))) == TRUE)
                {
                    addCmd("Client:\t[Incoming][" + ip + "]\t\t"+mess, ip);
                }
                break;
            case dcpp::DebugManager::CLIENT_OUT:
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("client_out_button"))) == TRUE)
                {
                    addCmd("Client:\t[Outgoing][" + ip + "]\t\t"+mess, ip);
                }
                break;
            default: break;
        }
}
void cmddebug::onScroll_gui(GtkAdjustment *adjustment, gpointer data)
{
    cmddebug *cmd = (cmddebug *)data;
    gdouble value = gtk_adjustment_get_value(adjustment);
#if GTK_CHECK_VERSION(3, 0, 0)
    cmd->scrollToBottom = value >= (gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size (adjustment));
#else
    cmd->scrollToBottom = value >= (adjustment->upper-adjustment->page_size);
#endif
}

void cmddebug::onResize_gui(GtkAdjustment *adjustment, gpointer data)
{
    cmddebug *cmd = (cmddebug *)data;
    gdouble value = gtk_adjustment_get_value(adjustment);
#if GTK_CHECK_VERSION(3, 0, 0)
    if (cmd->scrollToBottom && value < (gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size (adjustment)))
#else
    if (cmd->scrollToBottom && value < (adjustment->upper-adjustment->page_size))
#endif
    {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(cmd->buffer, &iter);
        gtk_text_buffer_move_mark(cmd->buffer, cmd->cmdMark, &iter);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(cmd->getWidget("cmdtextview")), cmd->cmdMark, 0, FALSE, 0, 0);
    }
}

void cmddebug::addCmd(const std::string& cmd,const std::string& ip) {
    string message;
    //g_print("CMD %s\n",cmd.c_str());
    //g_print("CMDSIZE %d\n",cmdList.size());

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("by_ip_button"))) == TRUE) {
        if (!strcmp(gtk_entry_get_text(GTK_ENTRY(getWidget("entrybyip"))), ip.c_str()))
            cmdList.push_back(cmd);
    }
    else
        cmdList.push_back(cmd);
    s.signal();
}

void cmddebug::on(dcpp::DebugManagerListener::DebugDetection, const std::string& com) noexcept
{
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(getWidget("detection_button"))) == TRUE)
        addCmd(com,"");
}
