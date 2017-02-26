#ifndef TEMPLATES_H
#define TEMPLATES_H

template<class T>
void CleanExpired(std::vector<std::weak_ptr<T>> vec){
	typename std::vector<std::weak_ptr<T>>::iterator it = vec.begin();
	while(it != vec.end()){
		if(it->expired()) {
			vec.erase(it);
		} else {
			++it;
		}
	}
}

#endif
