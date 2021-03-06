/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016, Technical University Kosice, Slovakia
 *  All rights reserved.
 *
 *  Author: Frantisek Durovsky
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Univ of CO, Boulder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(ros::NodeHandle *nh, int mode) :
    QMainWindow(0),
    ui(new Ui::MainWindow),
    mode(mode),
    timer_id(0),
    slider_trajectory_duration(5),
    spinbox_trajectory_duration(5)
{
    ui->setupUi(this);

    slider_goals.resize(NUM_OF_JOINTS);
    spinbox_goals.resize(NUM_OF_JOINTS);

    // Subscribe to joint_states
    sub_joint_states = nh->subscribe("/sef_roboter/joint_states", 1, &MainWindow::jointStateCallback, this);

    ROS_INFO("Mode %d", mode);

    if(mode == SIMULATION)
    {
        // Initialize publisher for simulation mode
        ROS_INFO("Initializing in simulation mode !");
        pub_joint_states = nh->advertise<sensor_msgs::JointState >("/sef_roboter/joint_states_goal",1);
        ui->label_mode->setText("<font color='green'>Gazebo Sim</font>");
    }
    else if(mode == REAL_ROBOT)
    {
        // Initialize publisher for real robot communication
        ROS_INFO("Initializing in real robot mode !");
        pub_joint_states = nh->advertise<sensor_msgs::JointState >("/sef_roboter/joint_states_goal",1);
        ui->label_mode->setText("<font color='red'>REAL ROBOT</font>");

        //Initialize service client for referencing purposes
        srv_reference_joint_client = nh->serviceClient<sef_roboter_ros_control::reference_joint>("reference_joint");
    }

    // Start timer
    timer_id = startTimer(10);
}

MainWindow::~MainWindow()
{

    if(timer_id)
        killTimer(timer_id);

    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *timer_event)
{
    ros::spinOnce();
}

void MainWindow::jointStateCallback(const sensor_msgs::JointStateConstPtr &msg)
{
    QString value_str;

    value_str = QString::number(msg->position[0], 'f', 4);
    ui->label_actual_value_joint_1->setText(value_str);

    value_str = QString::number(msg->position[1], 'f', 4);
    ui->label_actual_value_joint_2->setText(value_str);

    value_str = QString::number(msg->position[2], 'f', 4);
    ui->label_actual_value_joint_3->setText(value_str);

    value_str = QString::number(msg->position[3], 'f', 4);
    ui->label_actual_value_joint_4->setText(value_str);

    value_str = QString::number(msg->position[4], 'f', 4);
    ui->label_actual_value_joint_5->setText(value_str);

    value_str = QString::number(msg->position[5], 'f', 4);
    ui->label_actual_value_joint_6->setText(value_str);
}

void MainWindow::on_button_move_to_home_clicked()
{
    ROS_INFO("Moving to Home Position");

    sensor_msgs::JointState goal_state;
    goal_state.header.stamp = ros::Time::now();
    goal_state.header.frame_id = "base_link";
    goal_state.position.resize(NUM_OF_JOINTS);
    goal_state.position[0] = 0.0;
    goal_state.position[1] = 0.0;
    goal_state.position[2] = 0.0;
    goal_state.position[3] = 0.0;
    goal_state.position[4] = 0.0;
    goal_state.position[5] = 0.0;

    //Publish goal joint state
    pub_joint_states.publish(goal_state);
}

void MainWindow::on_button_move_to_slider_goal_clicked()
{
    ROS_INFO("Moving to Goal 1: [%f, %f, %f, %f, %f, %f]", slider_goals[0], slider_goals[1], slider_goals[2],
                                                           slider_goals[3], slider_goals[4], slider_goals[5]);

    sensor_msgs::JointState goal_state;
    goal_state.header.stamp = ros::Time::now();
    goal_state.header.frame_id = "base_link";
    goal_state.position.resize(NUM_OF_JOINTS);
    goal_state.position[0] = slider_goals[0];
    goal_state.position[1] = slider_goals[1];
    goal_state.position[2] = slider_goals[2];
    goal_state.position[3] = slider_goals[3];
    goal_state.position[4] = slider_goals[4];
    goal_state.position[5] = slider_goals[5];

    //Publish goal joint state
    pub_joint_states.publish(goal_state);
}

void MainWindow::on_button_move_to_spinbox_goal_clicked()
{
    ROS_INFO("Moving to Goal 2: [%f, %f, %f, %f, %f, %f]", spinbox_goals[0], spinbox_goals[1], spinbox_goals[2],
                                                           spinbox_goals[3], spinbox_goals[4], spinbox_goals[5]);

    sensor_msgs::JointState goal_state;
    goal_state.header.stamp = ros::Time::now();
    goal_state.header.frame_id = "base_link";
    goal_state.position.resize(NUM_OF_JOINTS);
    goal_state.position[0] = spinbox_goals[0];
    goal_state.position[1] = spinbox_goals[1];
    goal_state.position[2] = spinbox_goals[2];
    goal_state.position[3] = spinbox_goals[3];
    goal_state.position[4] = spinbox_goals[4];
    goal_state.position[5] = spinbox_goals[5];

    //Publish goal joint state
    pub_joint_states.publish(goal_state);
}

void MainWindow::on_slider_joint_1_valueChanged(int value)
{
    slider_goals[0] = double(value)/100;
    QString value_str = QString::number(slider_goals[0], 'f', 3);
    ui->label_slider_value_joint_1->setText(value_str);

}

void MainWindow::on_slider_joint_2_valueChanged(int value)
{
    slider_goals[1] = double(value)/100;
    QString value_str = QString::number(slider_goals[1], 'f', 3);
    ui->label_slider_value_joint_2->setText(value_str);
}

void MainWindow::on_slider_joint_3_valueChanged(int value)
{
    slider_goals[2] = double(value)/100;
    QString value_str = QString::number(slider_goals[2], 'f', 3);
    ui->label_slider_value_joint_3->setText(value_str);
}

void MainWindow::on_slider_joint_4_valueChanged(int value)
{
    slider_goals[3] = double(value)/100;
    QString value_str = QString::number(slider_goals[3], 'f', 3);
    ui->label_slider_value_joint_4->setText(value_str);
}

void MainWindow::on_slider_joint_5_valueChanged(int value)
{
    slider_goals[4] = double(value)/100;
    QString value_str = QString::number(slider_goals[4], 'f', 3);
    ui->label_slider_value_joint_5->setText(value_str);
}

void MainWindow::on_slider_joint_6_valueChanged(int value)
{
    slider_goals[5] = double(value)/100;
    QString value_str = QString::number(slider_goals[5], 'f', 3);
    ui->label_slider_value_joint_6->setText(value_str);
}

void MainWindow::on_spinbox_joint_1_valueChanged(double arg1)
{
    spinbox_goals[0] = arg1;
}

void MainWindow::on_spinbox_joint_2_valueChanged(double arg1)
{
    spinbox_goals[1] = arg1;
}

void MainWindow::on_spinbox_joint_3_valueChanged(double arg1)
{
    spinbox_goals[2] = arg1;
}

void MainWindow::on_spinbox_joint_4_valueChanged(double arg1)
{
    spinbox_goals[3] = arg1;
}

void MainWindow::on_spinbox_joint_5_valueChanged(double arg1)
{
    spinbox_goals[4] = arg1;
}

void MainWindow::on_spinbox_joint_6_valueChanged(double arg1)
{
    spinbox_goals[5] = arg1;
}

void MainWindow::on_spinbox_slider_duration_valueChanged(int arg1)
{
    slider_trajectory_duration = arg1;
}

void MainWindow::on_spinbox_spinbox_duration_valueChanged(int arg1)
{
    spinbox_trajectory_duration = arg1;
}

void MainWindow::on_button_reference_joint_1_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 1;

        ui->label_joint_blocked_1->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 1: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 1");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");
}

void MainWindow::on_button_reference_joint_2_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 2;

        ui->label_joint_blocked_2->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 2: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 2");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");
}

void MainWindow::on_button_reference_joint_3_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 3;

        ui->label_joint_blocked_3->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 3: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 3");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");

}

void MainWindow::on_button_reference_joint_4_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 4;

        ui->label_joint_blocked_4->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 4: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 4");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");

}

void MainWindow::on_button_reference_joint_5_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 5;

        ui->label_joint_blocked_5->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 5: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 5");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");

}

void MainWindow::on_button_reference_joint_6_clicked()
{
    if(mode == REAL_ROBOT)
    {
        sef_roboter_ros_control::reference_joint srv;
        srv.request.joint_number = 6;

        ui->label_joint_blocked_6->setText("<font color='red'>!!!</font>");

        if (srv_reference_joint_client.call(srv))
            ROS_INFO("Referencing Joint 6: %d", srv.response.accepted);
        else
            ROS_ERROR("Failed to call reference_joint for Joint 6");
    }
    else
        ROS_WARN("Referencing allowed only in REAL ROBOT mode");

}
