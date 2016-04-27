/*
*Team Unware Basketball Robot NWPU
*
*根据机器人当前信息在RVIZ中绘制出对应图形：
*1.铲子
*2.正前方向
*
*Author = liao-zhihan
*
*first_debug_date:2016-01-20
*测试通过
*/

#include <ros/ros.h>
#include <visualization_msgs/MarkerArray.h>

class RobotDirectionMarker
{
public:
    RobotDirectionMarker(ros::NodeHandle &node) ;
    ~RobotDirectionMarker()  ;

protected:
private:
    //定时器，触发一次则将队列中所有marker绘制在rviz中
    void timerCallBack(const ros::TimerEvent &event) ;
    //在机器人正前方画出方向
    void draw_direction() ;
    //根据实际画出铲子
    void draw_shovel() ;
    //删除所有已经画过的marker
    void deleteAllMaker() ;
private:
    double left_shovel_to_center_distance_ ;
    double right_shovel_to_center_distance_;
    double shovel_length_ ;
    double marker_z_;

    ros::NodeHandle nh_ ;
    ros::NodeHandle p_nh_ ;
    visualization_msgs::MarkerArray marker_array_ ;
    ros::Publisher marker_pub_ ;
    ros::Timer marker_pub_timer_ ;

};

RobotDirectionMarker::RobotDirectionMarker(ros::NodeHandle &node)
			:nh_(node),
 			p_nh_("~")
{
    p_nh_.param("left_shovel_to_center_distance",left_shovel_to_center_distance_,-0.115) ;
    p_nh_.param("right_shovel_to_center_distance",right_shovel_to_center_distance_,0.115) ;
    p_nh_.param("shovel_length" , shovel_length_ , 0.25+0.17) ;
    //0.17 : 指的是以机器人中心为圆心画内切圆的半径
    //0.25 : 篮球机器人铲子的长度
    marker_z_ = -0.1 ;

    marker_pub_timer_ = nh_.createTimer(ros::Duration(0.1),&RobotDirectionMarker::timerCallBack,this,false) ;

    marker_pub_ = nh_.advertise<visualization_msgs::MarkerArray>("robot_direction",10,false) ;

    draw_direction();
    draw_shovel();
}

RobotDirectionMarker::~RobotDirectionMarker()
{
    deleteAllMaker() ;
    nh_.shutdown();
}

void RobotDirectionMarker::deleteAllMaker()
{
    visualization_msgs::Marker shovel_marker ;
    visualization_msgs::Marker direction_marker ;
    direction_marker.action = visualization_msgs::Marker::DELETE ;
    shovel_marker.action = visualization_msgs::Marker::DELETE ;
    marker_array_.markers.push_back(direction_marker);
    marker_array_.markers.push_back(shovel_marker);
    marker_pub_.publish(marker_array_) ;
}

void RobotDirectionMarker::timerCallBack(const ros::TimerEvent &event)
{
   if(marker_pub_.getNumSubscribers() > 0)
   {
       for (size_t i = 0 ; i < marker_array_.markers.size() ; i++)
       {
            marker_array_.markers[i].header.stamp = ros::Time::now() ;
       }
       marker_pub_.publish(marker_array_) ;
    }
}
void RobotDirectionMarker::draw_direction()
{
    visualization_msgs::Marker direction_marker ;
    direction_marker.header.frame_id = "/base_link" ;
    direction_marker.type = visualization_msgs::Marker::LINE_LIST ;
    direction_marker.action = visualization_msgs::Marker::ADD ;

    direction_marker.color.r = 1.0 ;
    direction_marker.color.g = 0.0 ;
    direction_marker.color.b = 0.0 ;
    direction_marker.color.a = 1.0 ;
    direction_marker.scale.x = 0.01 ;
    direction_marker.ns = "robot_direction" ;
    direction_marker.pose.orientation.w = 1.0 ;

    geometry_msgs::Point point_tmp ;

    //draw the direction of the robot
    //start point
    point_tmp.x = 0.0 ;
    point_tmp.y = 0.0 ;
    point_tmp.z = marker_z_ ;
    direction_marker.points.push_back(point_tmp);
    //end point
    point_tmp.x = 0.7 ;
    direction_marker.points.push_back(point_tmp);
    marker_array_.markers.push_back(direction_marker);
}

void RobotDirectionMarker::draw_shovel()
{
    visualization_msgs::Marker shovel_marker ;
    shovel_marker.header.frame_id = "/base_link" ;
    shovel_marker.ns = "robot_shovel" ;
    shovel_marker.type = visualization_msgs::Marker::LINE_LIST ;
    shovel_marker.action = visualization_msgs::Marker::ADD ;

    shovel_marker.color.r = 0.0 ;
    shovel_marker.color.g = 0.0 ;
    shovel_marker.color.b = 1.0 ;
    shovel_marker.color.a = 1.0 ;
    shovel_marker.scale.x = 0.03 ;
    shovel_marker.pose.orientation.w = 1.0 ;

    std::vector<geometry_msgs::Point> point_vector ;

    geometry_msgs::Point point_tmp ;

    //draw the left shovel
    //start point
    point_tmp.x = 0.0 ;
    point_tmp.y = left_shovel_to_center_distance_ ;
    point_tmp.z = marker_z_ ;
    point_vector.push_back(point_tmp);
    //end point
    point_tmp.x = shovel_length_ ;
    point_vector.push_back(point_tmp);

    //draw the right shovel
    //start point
    point_tmp.x = 0.0 ;
    point_tmp.y = right_shovel_to_center_distance_ ;
    point_tmp.z = marker_z_ ;
    point_vector.push_back(point_tmp);
    //end point
    point_tmp.x = shovel_length_ ;
    point_vector.push_back(point_tmp);


    shovel_marker.points = point_vector ;
    marker_array_.markers.push_back(shovel_marker);
}


int main(int argc , char **argv)
{
    ros::init(argc , argv , "basketball_direction_marker") ;
    ros::NodeHandle node("basketball_direction_marker") ;
    RobotDirectionMarker robot_direction_marker(node) ;
    ros::spin() ;
    return  0 ;
}
