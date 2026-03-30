#include "HomePage.h"

GtkWidget* home_page_create() {
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // 顶部导航栏
    GtkWidget* nav_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start(nav_box, 20);
    gtk_widget_set_margin_top(nav_box, 20);
    gtk_widget_set_margin_bottom(nav_box, 30);

    GtkWidget* btn_home = gtk_button_new_with_label("首页");
    GtkWidget* btn_chat = gtk_button_new_with_label("聊天");
    GtkWidget* btn_mine = gtk_button_new_with_label("我的");

    gtk_box_append(GTK_BOX(nav_box), btn_home);
    gtk_box_append(GTK_BOX(nav_box), btn_chat);
    gtk_box_append(GTK_BOX(nav_box), btn_mine);

    // 占位提示
    GtkWidget* label = gtk_label_new("首页框架已就绪\n可二次开发");
    gtk_label_set_markup(GTK_LABEL(label), "<span size='large'>首页框架已就绪</span>");

    gtk_box_append(GTK_BOX(box), nav_box);
    gtk_box_append(GTK_BOX(box), label);

    return box;
}
