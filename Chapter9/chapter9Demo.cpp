#include "arrayQueue.hpp"
#include "circuitRouting.hpp"
#include "factorySimulation.hpp"
#include "imageComponents.hpp"
#include "railroad.hpp"

#include <iostream>

int main() {
    arrayQueue<int> circular(2);
    linkedQueue<int> linked;
    queue<int>& a = circular;
    queue<int>& b = linked;
    a.push(10); a.push(20);
    b.push(10); b.push(20);
    std::cout << "queue front/back: " << a.front() << '/' << a.back()
              << " (linked: " << b.front() << '/' << b.back() << ")\n";

    std::cout << "FIFO railroad [3,4,2,1] with two holding tracks:\n";
    printQueueRailroadMoves(rearrangeRailroadWithQueues({3, 4, 2, 1}, 2), std::cout);

    const std::vector<std::vector<int>> board = {
        {0, 0, 1}, {1, 0, 1}, {1, 0, 0}
    };
    std::cout << "shortest circuit route: ";
    for (const GridPoint& p : routeCircuit(board, {0, 0}, {2, 2}))
        std::cout << p << ' ';
    std::cout << '\n';

    const std::vector<std::vector<int>> image = {
        {1, 1, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 1}
    };
    const ComponentLabeling components = recognizeComponents(image);
    std::cout << "image components: " << components.count() << '\n';
    for (const std::vector<int>& row : components.labels) {
        for (int label : row) std::cout << label << ' ';
        std::cout << '\n';
    }

    const FactoryResult factory = simulateFactory(
        {{1, 0, 3}, {2, 1, 2}, {3, 1, 1}}, 2);
    std::cout << "factory schedule:\n";
    for (const FactoryRecord& record : factory.records)
        std::cout << "job " << record.id << " machine " << record.machine
                  << " start " << record.start << " finish " << record.finish
                  << " wait " << record.wait << '\n';
    std::cout << "makespan " << factory.makespan
              << ", average wait " << factory.averageWait << '\n';
}
