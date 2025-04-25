import axios from 'axios'
import config from './config'

const service = axios.create({
  baseURL: config.apiRoot,
  timeout: config.timeout,
  headers: config.headers,
})

service.interceptors.request.use(
  (config) => {
    // any tokens?
    return config
  },
  (error) => {
    console.error('Request error:', error)
    return Promise.reject(error)
  },
)

service.interceptors.response.use(
  (response) => {
    const res = response.data

    if (res.success === false) {
      console.error('API error:', res.message)
      return Promise.reject(new Error(res.message || 'Error'))
    }
    return res
  },
  (error) => {
    console.error('Response error:', error)
    return Promise.reject(error)
  },
)

export default service
