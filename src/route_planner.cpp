#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model)
{
    // scale the floats to percentages
    start_x *= 0.01f;
    start_y *= 0.01f;
    end_x *= 0.01f;
    end_y *= 0.01f;

    // find the closest nodes to (start_x, start_y)
    start_node = &m_Model.FindClosestNode(start_x, start_y);

    // find the closest nodes to (end_x, end_y)
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}
